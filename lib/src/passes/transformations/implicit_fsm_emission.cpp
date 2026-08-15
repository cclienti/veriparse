// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "implicit_fsm_detail.hpp"

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

using namespace FsmDetail;

int ImplicitFsmElaboration::build_decode(const std::vector<State> &states,
                                         const std::vector<Transition> &entry,
                                         std::size_t entry_next)
{
    if(m_proc.decoded.empty()) {
        if(m_proc.encoding == Encoding::OUTPUT) {
            LOG_ERROR_N(m_proc.pragmalist)
                << "veriparse_encoding = \"output\" with no decoded output: the state "
                << "bits carry the outputs, and there are none — the hint is inert";
            return 1;
        }
        return 0;
    }

    m_proc.init_decode = entry.front().decode;

    const std::size_t total = states.size() + (m_proc.hold_needed ? 1 : 0);
    m_proc.decode_arms.assign(total, {});

    // Arrivals at each state, transition order kept; the init segment is
    // the arrival into the entry state (§6.2).
    struct Arrival
    {
        std::size_t source;
        const Transition *transition;
    };
    std::vector<std::vector<Arrival>> arrivals(total);
    for(std::size_t i = 0; i < states.size(); ++i) {
        for(const auto &transition : states[i].out) {
            if(transition.next < total) {
                arrivals[transition.next].push_back({i, &transition});
            }
        }
    }
    if(entry_next < total) {
        arrivals[entry_next].push_back({k_entry, &entry.front()});
    }

    // §6.2 stability: an arm re-evaluates over post-edge values what the
    // source read at the edge, so no operand may be a register the same
    // arriving path commits, an input, or anything driven outside this
    // process — looking through wires, §6.1's and the author's alike,
    // which substitute down to those leaves.
    std::map<std::string, AST::Node::Ptr> wire_values;
    for(const auto &wire : m_proc.wires) {
        wire_values[wire.name] = wire.value;
    }
    std::set<std::string> input_names;
    if(m_proc.module->get_ports()) {
        for(const auto &port : *m_proc.module->get_ports()) {
            if(port->get_decl() && (port->get_direction() == AST::Port::DirectionEnum::INPUT ||
                                    port->get_direction() == AST::Port::DirectionEnum::INOUT)) {
                input_names.insert(port->get_decl()->get_name());
            }
        }
    }
    // The author's continuous assigns and net initializers join the wire
    // map: the look-through reaches their leaves, where the verdict falls.
    if(m_proc.module->get_items()) {
        for(const auto &item : *m_proc.module->get_items()) {
            if(item->is_node_type(AST::NodeType::Assign)) {
                const auto &assign = AST::cast_to<AST::Assign>(item);
                const auto &target = lvalue_target(assign->get_left());
                if(!target.empty() && assign->get_right() && assign->get_right()->get_var() &&
                   !wire_values.count(target)) {
                    wire_values[target] = AST::to_node(assign->get_right()->get_var());
                }
            } else if(const auto &net = std::dynamic_pointer_cast<AST::Net>(item)) {
                if(net->get_cont_assign() && net->get_cont_assign()->get_var() &&
                   !wire_values.count(net->get_name())) {
                    wire_values[net->get_name()] = AST::to_node(net->get_cont_assign()->get_var());
                }
            }
        }
    }
    // Everything driven outside this process — instance outputs and
    // generate regions included — by the same walk §9.2.2.4 trusts.
    std::set<std::string> foreign;
    collect_foreign_drivers(m_proc.module, m_proc.pragmalist, m_modules, foreign);
    const auto operands_of = [&](const AST::Node::Ptr &node) {
        std::set<std::string> raw;
        collect_identifier_names(node, raw);
        std::set<std::string> names;
        std::vector<std::string> todo(raw.begin(), raw.end());
        while(!todo.empty()) {
            const std::string name = todo.back();
            todo.pop_back();
            if(!names.insert(name).second) {
                continue;
            }
            const auto &wire = wire_values.find(name);
            if(wire != wire_values.end()) {
                std::set<std::string> inner;
                collect_identifier_names(wire->second, inner);
                todo.insert(todo.end(), inner.begin(), inner.end());
            }
        }
        return names;
    };
    const auto check_stable =
        [&](const AST::Node::Ptr &expr, const std::set<std::string> &commits,
            const std::string &name, const char *what) {
            for(const auto &operand : operands_of(expr)) {
                // A wire was expanded to its leaves: the verdict falls on
                // them, not on its name.
                if(wire_values.count(operand)) {
                    continue;
                }
                // An input may change on the arrival edge itself: the
                // always_ff read it before the edge where the emitted arm
                // re-reads it after — measured divergent, not a style
                // rule. A register of this process is stable for the
                // whole arrived cycle; one committed by anything outside
                // it changes on that edge like the input it usually is.
                if(input_names.count(operand)) {
                    LOG_ERROR_N(expr)
                        << "decoded output '" << name << "': its " << what << " reads input '"
                        << operand << "', which can change on the arrival edge — the "
                        << "always_ff sampled it before the edge, the emitted arm re-reads "
                        << "it after; register the input first, in this process";
                    return 1;
                }
                if(foreign.count(operand)) {
                    LOG_ERROR_N(expr)
                        << "decoded output '" << name << "': its " << what << " reads '" << operand
                        << "', which is driven outside this process and can "
                        << "change on the arrival edge like an input — register it in "
                        << "this process";
                    return 1;
                }
                if(commits.count(operand)) {
                    LOG_ERROR_N(expr)
                        << "decoded output '" << name << "': its " << what << " reads '" << operand
                        << "', which the same arriving path commits — the emitted arm would "
                        << "track the new value where the source held the entry value; keep "
                        << "the output a register, or commit '" << operand << "' on another path";
                    return 1;
                }
                if(m_proc.decoded.count(operand)) {
                    LOG_ERROR_N(expr)
                        << "decoded output '" << name << "': its " << what << " reads decoded "
                        << "output '" << operand << "' from the previous cycle, which the "
                        << "emitted comb cannot show — assign '" << operand
                        << "' first, or keep one of them a register";
                    return 1;
                }
            }
            return 0;
        };

    Generators::VerilogGenerator renderer;
    for(std::size_t state = 0; state < total; ++state) {
        if(arrivals[state].empty()) {
            continue;
        }
        std::vector<std::size_t> sources;
        for(const auto &arrival : arrivals[state]) {
            if(std::find(sources.begin(), sources.end(), arrival.source) == sources.end()) {
                sources.push_back(arrival.source);
            }
        }
        for(const auto &elt : m_proc.decoded) {
            const std::string &name = elt.first;
            std::vector<std::pair<AST::Node::Ptr, AST::Node::Ptr>> arm;
            std::string arm_render;
            bool first_group = true;
            for(const auto &source : sources) {
                std::vector<const Transition *> group;
                for(const auto &arrival : arrivals[state]) {
                    if(arrival.source == source) {
                        group.push_back(arrival.transition);
                    }
                }
                bool all_equal = true;
                const std::string first_value = renderer.render(group.front()->decode.at(name));
                for(const auto &transition : group) {
                    if(renderer.render(transition->decode.at(name)) != first_value) {
                        all_equal = false;
                        break;
                    }
                }
                std::vector<std::pair<AST::Node::Ptr, AST::Node::Ptr>> candidate;
                std::vector<const Transition *> survivors;
                if(all_equal) {
                    candidate.push_back({nullptr, group.front()->decode.at(name)});
                    survivors.assign(group.begin(), group.end());
                } else {
                    // Only the conjuncts that differ within the group
                    // discriminate its legs: the shared prefix (the way
                    // into the fork, a wait-state's exit condition) holds
                    // on every leg and is dropped before the tree — and
                    // before stability judges the guards.
                    std::vector<std::vector<AST::Node::Ptr>> conjuncts;
                    for(const auto &transition : group) {
                        std::vector<AST::Node::Ptr> terms;
                        flatten_land(transition->guard, terms);
                        conjuncts.push_back(terms);
                    }
                    std::set<std::string> common;
                    for(const auto &term : conjuncts.front()) {
                        common.insert(renderer.render(term));
                    }
                    for(std::size_t g = 1; g < conjuncts.size(); ++g) {
                        std::set<std::string> here;
                        for(const auto &term : conjuncts[g]) {
                            here.insert(renderer.render(term));
                        }
                        for(auto it = common.begin(); it != common.end();) {
                            it = here.count(*it) ? std::next(it) : common.erase(it);
                        }
                    }
                    // First match wins in the source (§12.5), so after
                    // the reduction a leg repeating an earlier guard is
                    // unreachable — duplicate case items — and a leg with
                    // no residual guard is the else, making later legs
                    // unreachable too. Prune both, keeping the surviving
                    // legs paired with their transitions.
                    std::set<std::string> seen_guards;
                    for(std::size_t g = 0; g < group.size(); ++g) {
                        AST::Node::Ptr reduced;
                        for(const auto &term : conjuncts[g]) {
                            if(common.count(renderer.render(term))) {
                                continue;
                            }
                            reduced = reduced ? conjoin(reduced, term, term->get_filename(),
                                                        term->get_line())
                                              : term;
                        }
                        if(reduced && !seen_guards.insert(renderer.render(reduced)).second) {
                            continue;
                        }
                        candidate.push_back({reduced, group[g]->decode.at(name)});
                        survivors.push_back(group[g]);
                        if(!reduced) {
                            break;
                        }
                    }
                }
                for(std::size_t g = 0; g < survivors.size(); ++g) {
                    std::set<std::string> commits;
                    for(const auto &stmt : *survivors[g]->action) {
                        collect_driven(stmt, commits);
                    }
                    if(check_stable(survivors[g]->decode.at(name), commits, name, "value")) {
                        return 1;
                    }
                    if(!all_equal && candidate[g].first &&
                       check_stable(candidate[g].first, commits, name, "arrival guard")) {
                        return 1;
                    }
                }
                std::string candidate_render;
                for(const auto &leg : candidate) {
                    candidate_render += (leg.first ? renderer.render(leg.first) : "") + "->" +
                                        renderer.render(leg.second) + ";";
                }
                if(first_group) {
                    arm = candidate;
                    arm_render = candidate_render;
                    first_group = false;
                } else if(candidate_render != arm_render) {
                    // §6.2 coherency: the state register cannot tell the
                    // arrivals apart, so their decode must agree.
                    LOG_ERROR_N(elt.second)
                        << "decoded output '" << name << "': paths from different states "
                        << "arrive at one state with different values — the state register "
                        << "cannot tell them apart; make the arrivals agree, or keep the "
                        << "output a register";
                    return 1;
                }
            }
            m_proc.decode_arms[state][name] = arm;
        }
    }

    // §6.2 output encoding: the state bits ARE the outputs. Legal exactly
    // when every arm is one constant per state — totality made the value
    // a function of the state, this asks it to be a literal one — with a
    // disambiguation field separating states that share an output vector.
    if(m_proc.encoding == Encoding::OUTPUT) {
        unsigned int lsb = 0;
        for(const auto &elt : m_proc.decoded) {
            const auto &decl = std::dynamic_pointer_cast<AST::Declaration>(elt.second);
            unsigned int width = 0;
            if(!decl || declared_width(decl, width) || width < 1) {
                LOG_ERROR_N(elt.second)
                    << "output encoding: cannot size decoded output '" << elt.first
                    << "': its declaration or packed range is not resolvable";
                return 1;
            }
            m_proc.output_slices.push_back({elt.first, lsb, width});
            lsb += width;
        }
        const unsigned int out_width = lsb;
        // Bignum composition end to end: a shift never exceeds a host
        // integer's width, whatever the platform's long is — the one cap
        // that matters is checked before anything converts.
        std::vector<mpz_class> vectors(m_proc.decode_arms.size(), mpz_class(0));
        std::vector<bool> unreachable(m_proc.decode_arms.size(), false);
        for(std::size_t state = 0; state < m_proc.decode_arms.size(); ++state) {
            // A state no path reaches — a constant-zero repeat's body —
            // has no arrival values: it takes the entry vector below,
            // like the always_comb's default arm would have served it.
            if(m_proc.decode_arms[state].empty()) {
                unreachable[state] = true;
                continue;
            }
            for(const auto &slice : m_proc.output_slices) {
                const auto &arm = m_proc.decode_arms[state].find(std::get<0>(slice));
                if(arm == m_proc.decode_arms[state].end()) {
                    LOG_ERROR_N(m_proc.pragmalist)
                        << "output encoding: no decode value for '" << std::get<0>(slice)
                        << "' in a state — please report this input";
                    return 1;
                }
                const auto &chain = arm->second;
                if(chain.size() != 1 || chain.front().first) {
                    LOG_ERROR_N(chain.front().second)
                        << "output encoding: decoded output '" << std::get<0>(slice)
                        << "' takes different values within one state — a state bit is "
                        << "one value per state; make the arrivals agree, or pick "
                        << "binary/one_hot/gray";
                    return 1;
                }
                // A literal, not a folded expression: the evaluator's
                // bignum arithmetic does not follow IEEE §11.8.2's
                // self-determined sizing, and a wrong wrap baked into a
                // state constant would diverge silently where the other
                // encodings emit the expression verbatim.
                if(!chain.front().second->is_node_type(AST::NodeType::IntConstN)) {
                    LOG_ERROR_N(chain.front().second)
                        << "output encoding: decoded output '" << std::get<0>(slice)
                        << "' is not a literal in every state — a state bit is a "
                        << "literal; pick binary/one_hot/gray, or keep the output a "
                        << "register";
                    return 1;
                }
                const mpz_class folded =
                    AST::cast_to<AST::IntConstN>(chain.front().second)->get_value();
                const unsigned int width = std::get<2>(slice);
                const mpz_class modulus = mpz_class(1) << width;
                const mpz_class masked = ((folded % modulus) + modulus) % modulus;
                vectors[state] |= masked << std::get<1>(slice);
            }
        }
        if(entry_next < vectors.size()) {
            for(std::size_t state = 0; state < vectors.size(); ++state) {
                if(unreachable[state]) {
                    vectors[state] = vectors[entry_next];
                }
            }
        }
        std::map<mpz_class, unsigned int> multiplicity;
        std::vector<unsigned int> disamb(vectors.size(), 0);
        unsigned int largest = 0;
        for(std::size_t state = 0; state < vectors.size(); ++state) {
            disamb[state] = multiplicity[vectors[state]]++;
            largest = std::max(largest, multiplicity[vectors[state]]);
        }
        const unsigned int d_width = largest > 1 ? clog2(largest) : 0;
        if(out_width + d_width > 32) {
            LOG_ERROR_N(m_proc.pragmalist)
                << "output encoding beyond 32 state bits (" << out_width << " output + " << d_width
                << " disambiguation): pick binary or gray";
            return 1;
        }
        m_proc.output_width = out_width + d_width;
        for(std::size_t state = 0; state < vectors.size(); ++state) {
            const mpz_class composed = vectors[state] | (mpz_class(disamb[state]) << out_width);
            m_proc.output_values.push_back(composed.convert_to<unsigned int>());
        }
    }
    return 0;
}

AST::Node::Ptr ImplicitFsmElaboration::emit_decode(const std::string &state_reg,
                                                   const std::vector<std::string> &state_names,
                                                   const std::string &reset_name, bool active_low,
                                                   const std::string &fn, int ln) const
{
    // The default arm carries the init values, so unreachable encodings
    // latch nothing; the reset branch mirrors §5, read as a level — a comb
    // has no edge for the reset kind to matter to (§6.2).
    const auto &make_values = [&](const std::map<std::string, AST::Node::Ptr> &values) {
        const auto &stmts = std::make_shared<AST::Node::List>();
        for(const auto &elt : m_proc.decoded) {
            stmts->push_back(AST::to_node(make_blocking(elt.first, values.at(elt.first), fn, ln)));
        }
        return AST::to_node(std::make_shared<AST::Block>(stmts, "", fn, ln));
    };

    const auto &caselist = std::make_shared<AST::Case::List>();
    for(std::size_t state = 0; state < m_proc.decode_arms.size(); ++state) {
        if(m_proc.decode_arms[state].empty()) {
            continue;
        }
        const auto &stmts = std::make_shared<AST::Node::List>();
        for(const auto &elt : m_proc.decode_arms[state]) {
            const auto &chain = elt.second;
            if(chain.size() == 1 && !chain.front().first) {
                stmts->push_back(
                    AST::to_node(make_blocking(elt.first, chain.front().second, fn, ln)));
                continue;
            }
            // if (g1) y = v1; else if (g2) ... else y = vn; — the guards
            // partition by construction (§C.3), the last leg is the else.
            AST::Node::Ptr tree =
                AST::to_node(make_blocking(elt.first, chain.back().second, fn, ln));
            for(std::size_t leg = chain.size() - 1; leg-- > 0;) {
                auto branch = std::make_shared<AST::IfStatement>(fn, ln);
                branch->set_cond(chain[leg].first->clone());
                branch->set_true_statement(
                    AST::to_node(make_blocking(elt.first, chain[leg].second, fn, ln)));
                branch->set_false_statement(tree);
                tree = AST::to_node(branch);
            }
            stmts->push_back(tree);
        }
        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names[state], fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(AST::to_node(std::make_shared<AST::Block>(stmts, "", fn, ln)));
        caselist->push_back(arm);
    }
    {
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_statement(make_values(m_proc.init_decode));
        caselist->push_back(arm);
    }

    auto case_stmt = std::make_shared<AST::CaseStatement>(fn, ln);
    case_stmt->set_comp(AST::to_node(make_id(state_reg, fn, ln)));
    case_stmt->set_caselist(caselist);

    AST::Node::Ptr reset_cond = AST::to_node(make_id(reset_name, fn, ln));
    if(active_low) {
        reset_cond = make_ulnot(reset_cond, fn, ln);
    }
    auto guard = std::make_shared<AST::IfStatement>(fn, ln);
    guard->set_cond(reset_cond);
    guard->set_true_statement(make_values(m_proc.init_decode));
    guard->set_false_statement(AST::to_node(case_stmt));

    auto comb = std::make_shared<AST::AlwaysComb>(fn, ln);
    comb->set_statement(AST::to_node(guard));
    return AST::to_node(comb);
}

AST::Node::ListPtr ImplicitFsmElaboration::emit(
    const AST::Module::Ptr &module, const AST::Sens::Ptr &clock, const AST::Node::Ptr &enable,
    const std::string &reset_name, bool active_low, const AST::Node::ListPtr &init_stmts,
    const std::vector<State> &states, std::size_t entry_next, const std::string &prefix)
{
    const std::string &fn = module->get_filename();
    const int ln = module->get_line();

    // One state per wait, plus the hold state a one-shot parks in (§2) —
    // omitted when no path ends the process, as none does in a perpetual
    // machine: an unreachable state would still cost encoding width. The
    // walk flagged it when recording a transition to the hold index.
    const bool hold_needed = m_proc.hold_needed;
    const std::size_t nstates = states.size() + (hold_needed ? 1 : 0);

    // §3: the encoding shapes the constants and the register width —
    // binary and gray pack into clog2 bits, one-hot spends one per state.
    if(m_proc.encoding == Encoding::ONE_HOT && nstates > 32) {
        LOG_ERROR_N(module) << "one_hot encoding beyond 32 states (" << nstates
                            << "): pick binary or gray";
        return nullptr;
    }
    const unsigned int width =
        m_proc.encoding == Encoding::ONE_HOT
            ? static_cast<unsigned int>(nstates)
            : (m_proc.encoding == Encoding::OUTPUT ? m_proc.output_width
                                                   : clog2(static_cast<unsigned int>(nstates)));
    const auto encode = [&](std::size_t index) -> unsigned int {
        switch(m_proc.encoding) {
        case Encoding::ONE_HOT:
            return 1U << index;
        case Encoding::GRAY:
            return static_cast<unsigned int>(index ^ (index >> 1));
        case Encoding::OUTPUT:
            // §6.2: the composed {disambiguation, outputs} value.
            return m_proc.output_values[index];
        case Encoding::BINARY:
        default:
            return static_cast<unsigned int>(index);
        }
    };

    // §10.1: a stem naming one state names it outright, several take the
    // stem with an ordinal, and an unlabelled state keeps the global
    // ordinal — naming is incremental.
    std::vector<std::string> state_names;
    {
        std::map<std::string, std::size_t> stem_total, stem_seen;
        for(const auto &state : states) {
            if(!state.stem.empty()) {
                ++stem_total[state.stem];
            }
        }
        for(std::size_t i = 0; i < states.size(); ++i) {
            const auto &stem = states[i].stem;
            if(stem.empty()) {
                state_names.push_back(prefix + "_state_" + std::to_string(i));
            } else if(stem_total[stem] == 1) {
                state_names.push_back(prefix + "_" + stem);
            } else {
                state_names.push_back(prefix + "_" + stem + "_" +
                                      std::to_string(stem_seen[stem]++));
            }
        }
    }
    if(hold_needed) {
        state_names.push_back(prefix + "_hold");
    }
    {
        // The ordinal scheme was collision-free by construction; label
        // stems must be checked against each other AND against the other
        // generated declarations of this machine.
        std::set<std::string> unique(state_names.begin(), state_names.end());
        std::size_t expected = state_names.size() + 1;
        unique.insert(prefix + "_state");
        for(const auto &wire : m_proc.wires) {
            unique.insert(wire.name);
            ++expected;
        }
        if(unique.size() != expected) {
            LOG_ERROR_N(module) << "a state name collides with another state or with a "
                                << "generated declaration (the state register or the "
                                << "countdown): rename the label";
            return nullptr;
        }
    }
    const std::string state_reg = prefix + "_state";

    // §10: a collision remaining after prefixing is an error, not a rename.
    Analysis::UniqueDeclaration::IdentifierSet declared;
    Analysis::UniqueDeclaration::analyze(AST::to_node(module), declared);
    for(const auto &name : state_names) {
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << name
                                << "' collides with an existing one";
            return nullptr;
        }
    }
    if(Analysis::UniqueDeclaration::identifier_declaration_exists(state_reg, declared)) {
        LOG_ERROR_N(module) << "generated declaration '" << state_reg
                            << "' collides with an existing one";
        return nullptr;
    }

    const auto &result = std::make_shared<AST::Node::List>();

    // localparam [w-1:0] <prefix>_state_<i> = <w>'d<i>;
    for(std::size_t i = 0; i < nstates; ++i) {
        auto type = std::make_shared<AST::ImplicitType>(fn, ln);
        type->set_packed_dims(make_packed_range(width - 1, fn, ln));
        auto param = std::make_shared<AST::Param>(fn, ln);
        param->set_name(state_names[i]);
        param->set_is_local(true);
        param->set_type(type);
        param->set_value(AST::to_node(make_const(encode(i), static_cast<int>(width), fn, ln)));
        result->push_back(AST::to_node(param));
    }

    // logic [w-1:0] <prefix>_state;
    auto reg_type = std::make_shared<AST::LogicType>(fn, ln);
    reg_type->set_packed_dims(make_packed_range(width - 1, fn, ln));
    auto reg = std::make_shared<AST::Var>(fn, ln);
    reg->set_name(state_reg);
    reg->set_type(reg_type);
    result->push_back(AST::to_node(reg));

    // §6.1: the materialized temporaries — a wire per surviving value,
    // typed by the temporary's declaration so the declared width keeps
    // truncating exactly as the source did, driven by one continuous
    // assign, read from the case arms.
    for(const auto &wire : m_proc.wires) {
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(wire.name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << wire.name
                                << "' collides with an existing one";
            return nullptr;
        }
        auto net = std::make_shared<AST::WireNet>(fn, ln);
        net->set_name(wire.name);
        // Dims plus signing, no data-type keyword: `wire signed [8:0]`
        // reads the same in SystemVerilog and in 1364 mode, and the
        // declared type is the whole point (§6.1, §11.6). A keyword-width
        // type (int, byte, ...) contributes its width and its default
        // signedness.
        {
            const auto &temp_type = wire.temp ? wire.temp->get_type() : nullptr;
            auto net_type = std::make_shared<AST::ImplicitType>(fn, ln);
            if(temp_type && temp_type->get_packed_dims() &&
               !temp_type->get_packed_dims()->empty()) {
                auto dims = std::make_shared<AST::Dimension::List>();
                for(const auto &dim : *temp_type->get_packed_dims()) {
                    dims->push_back(AST::cast_to<AST::Dimension>(dim->clone()));
                }
                net_type->set_packed_dims(dims);
            } else if(wire.temp) {
                unsigned int width = 0;
                if(!declared_width(wire.temp, width) && width > 1) {
                    net_type->set_packed_dims(make_packed_range(width - 1, fn, ln));
                }
            }
            const bool atom_signed =
                temp_type && (temp_type->is_node_type(AST::NodeType::IntType) ||
                              temp_type->is_node_type(AST::NodeType::IntegerType) ||
                              temp_type->is_node_type(AST::NodeType::ByteType) ||
                              temp_type->is_node_type(AST::NodeType::ShortintType) ||
                              temp_type->is_node_type(AST::NodeType::LongintType));
            const bool is_signed =
                temp_type &&
                (temp_type->get_signing() == AST::DataType::SigningEnum::SIGNED ||
                 (atom_signed && temp_type->get_signing() == AST::DataType::SigningEnum::NONE));
            if(is_signed) {
                net_type->set_signing(AST::DataType::SigningEnum::SIGNED);
            }
            net->set_type(net_type);
        }
        result->push_back(AST::to_node(net));
        auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
        lvalue->set_var(AST::to_node(make_id(wire.name, fn, ln)));
        auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
        rvalue->set_var(wire.value->clone());
        auto cont = std::make_shared<AST::Assign>(fn, ln);
        cont->set_left(lvalue);
        cont->set_right(rvalue);
        result->push_back(AST::to_node(cont));
    }

    // §6.2: the decoded outputs — an always_comb over the state register,
    // or under output encoding each output IS its slice of the register:
    // no decode gates and no reset gating either: the entry state's bits
    // ARE the init values (coherency), so the outputs behave exactly like
    // the registered outputs of a sync-reset machine — defined from the
    // first reset edge, holding through a mid-run re-assert until its
    // edge, which §5.2 keeps out of scope anyway. A level mux here would
    // put the reset on a combinational arc to every output for a window
    // the model does not define. The always block (never an assign, which
    // a 1364 reg cannot take) keeps both output modes legal.
    if(!m_proc.decoded.empty()) {
        if(m_proc.encoding == Encoding::OUTPUT) {
            const auto &slice_block = std::make_shared<AST::Node::List>();
            for(const auto &slice : m_proc.output_slices) {
                const unsigned int low = std::get<1>(slice);
                const unsigned int w = std::get<2>(slice);
                AST::Node::Ptr select;
                if(w == 1) {
                    select = AST::to_node(std::make_shared<AST::Pointer>(
                        AST::to_node(make_const(low, -1, fn, ln)),
                        AST::to_node(make_id(state_reg, fn, ln)), fn, ln));
                } else {
                    select = AST::to_node(std::make_shared<AST::Partselect>(
                        AST::to_node(make_const(low + w - 1, -1, fn, ln)),
                        AST::to_node(make_const(low, -1, fn, ln)),
                        AST::to_node(make_id(state_reg, fn, ln)), fn, ln));
                }
                slice_block->push_back(
                    AST::to_node(make_blocking(std::get<0>(slice), select, fn, ln)));
            }
            auto comb = std::make_shared<AST::AlwaysComb>(fn, ln);
            comb->set_statement(
                AST::to_node(std::make_shared<AST::Block>(slice_block, "", fn, ln)));
            result->push_back(AST::to_node(comb));
        } else {
            result->push_back(emit_decode(state_reg, state_names, reset_name, active_low, fn, ln));
        }
    }

    // Reset branch: the init segment verbatim, plus the state register
    // going to the first state (§5.1).
    const auto &reset_stmts = std::make_shared<AST::Node::List>();
    for(const auto &stmt : *init_stmts) {
        reset_stmts->push_back(stmt->clone());
    }
    reset_stmts->push_back(
        AST::to_node(make_state_assign(state_reg, state_names[entry_next], fn, ln)));
    auto reset_block = std::make_shared<AST::Block>(reset_stmts, "", fn, ln);

    // case (<state>) arms: one leg per outgoing transition — action plus
    // next state — chained if / else if / else, in enumeration order, the
    // last leg as the bare else since the guards partition by construction
    // (§C.3). The hold state carries no update and stays put.
    const auto &caselist = std::make_shared<AST::Case::List>();
    for(std::size_t i = 0; i < states.size(); ++i) {
        const auto &out = states[i].out;
        auto make_leg = [&](const Transition &transition) {
            const auto &leg_stmts = std::make_shared<AST::Node::List>();
            for(const auto &stmt : *transition.action) {
                leg_stmts->push_back(stmt->clone());
            }
            leg_stmts->push_back(
                AST::to_node(make_state_assign(state_reg, state_names[transition.next], fn, ln)));
            return AST::to_node(std::make_shared<AST::Block>(leg_stmts, "", fn, ln));
        };

        // A state no path reaches — a constant-zero repeat's body — keeps
        // an empty arm, like the hold state.
        if(out.empty()) {
            const auto &conds = std::make_shared<AST::Node::List>();
            conds->push_back(AST::to_node(make_id(state_names[i], fn, ln)));
            auto arm = std::make_shared<AST::Case>(fn, ln);
            arm->set_cond(conds);
            arm->set_statement(AST::to_node(
                std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "", fn, ln)));
            caselist->push_back(arm);
            continue;
        }

        AST::Node::Ptr body = make_leg(out.back());
        for(std::size_t j = out.size() - 1; j-- > 0;) {
            // Guards partition by construction: every non-final transition
            // of a multi-way state carries one. Refuse to emit otherwise.
            if(!out[j].guard) {
                LOG_ERROR_N(states[i].wait)
                    << "unguarded transition in a multi-way state: the path cover "
                    << "lost a fork condition — please report this input";
                return nullptr;
            }
            auto chain = std::make_shared<AST::IfStatement>(fn, ln);
            chain->set_cond(out[j].guard->clone());
            chain->set_true_statement(make_leg(out[j]));
            chain->set_false_statement(body);
            body = AST::to_node(chain);
        }

        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names[i], fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(body);
        caselist->push_back(arm);
    }
    if(hold_needed) {
        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names.back(), fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(AST::to_node(
            std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "", fn, ln)));
        caselist->push_back(arm);
    }

    auto case_stmt = std::make_shared<AST::CaseStatement>(fn, ln);
    case_stmt->set_comp(AST::to_node(make_id(state_reg, fn, ln)));
    case_stmt->set_caselist(caselist);

    // if (!rst_n) <reset> else [if (en)] case (...) — the enable gates the
    // case and nothing else: the reset is never gated, so the machine leaves
    // reset whether or not anything is running it (§5.3).
    AST::Node::Ptr else_branch = AST::to_node(case_stmt);
    if(enable) {
        auto gate = std::make_shared<AST::IfStatement>(fn, ln);
        gate->set_cond(enable->clone());
        gate->set_true_statement(else_branch);
        else_branch = AST::to_node(gate);
    }

    AST::Node::Ptr reset_cond = AST::to_node(make_id(reset_name, fn, ln));
    if(active_low) {
        auto ulnot = std::make_shared<AST::Ulnot>(fn, ln);
        ulnot->set_right(reset_cond);
        reset_cond = AST::to_node(ulnot);
    }
    auto guard = std::make_shared<AST::IfStatement>(fn, ln);
    guard->set_cond(reset_cond);
    guard->set_true_statement(AST::to_node(reset_block));
    guard->set_false_statement(else_branch);

    auto clock_sens = std::make_shared<AST::Sens>(fn, ln);
    clock_sens->set_type(clock->get_type());
    clock_sens->set_sig(clock->get_sig()->clone());
    const auto &sens_list = std::make_shared<AST::Sens::List>();
    sens_list->push_back(clock_sens);

    // §3: veriparse_reset_kind = "async" adds the reset's own edge to the
    // sensitivity; the guard structure is the same either way.
    if(m_proc.async_reset) {
        auto reset_sens = std::make_shared<AST::Sens>(fn, ln);
        reset_sens->set_type(active_low ? AST::Sens::TypeEnum::NEGEDGE
                                        : AST::Sens::TypeEnum::POSEDGE);
        reset_sens->set_sig(AST::to_node(make_id(reset_name, fn, ln)));
        sens_list->push_back(reset_sens);
    }

    auto always = std::make_shared<AST::AlwaysFF>(fn, ln);
    always->set_senslist(std::make_shared<AST::Senslist>(sens_list, fn, ln));
    always->set_statement(AST::to_node(guard));
    result->push_back(AST::to_node(always));

    // §10.2: record what the source does not state, for the state map and
    // the graphviz view — encoding, naming, the reset contract, and the
    // transition structure over the resolved names.
    if(m_report) {
        FsmReport::Process process;
        process.module_name = module->get_name();
        process.state_variable = state_reg;
        process.width = width;
        process.encoding =
            m_proc.encoding == Encoding::ONE_HOT
                ? "one_hot"
                : (m_proc.encoding == Encoding::GRAY
                       ? "gray"
                       : (m_proc.encoding == Encoding::OUTPUT ? "output" : "binary"));
        process.entry = state_names[entry_next];
        process.has_hold = hold_needed;
        process.reset_signal = reset_name;
        process.reset_active_level = active_low ? 0 : 1;
        process.reset_kind = m_proc.async_reset ? "async" : "sync";
        for(const auto &stmt : *init_stmts) {
            if(stmt->is_node_type(AST::NodeType::NonblockingSubstitution)) {
                process.reset_registers.push_back(
                    nba_target(AST::cast_to<AST::NonblockingSubstitution>(stmt)));
            }
        }
        process.reset_registers.push_back(state_reg);
        for(std::size_t i = 0; i < nstates; ++i) {
            FsmReport::State entry_state;
            entry_state.name = state_names[i];
            entry_state.value = encode(i);
            entry_state.line = i < states.size() ? states[i].wait->get_line() : ln;
            process.states.push_back(entry_state);
        }
        Generators::VerilogGenerator renderer;
        for(const auto &elt : m_proc.decoded) {
            FsmReport::Decode decode;
            decode.signal = elt.first;
            for(std::size_t i = 0; i < m_proc.decode_arms.size(); ++i) {
                const auto &arm = m_proc.decode_arms[i].find(elt.first);
                if(arm == m_proc.decode_arms[i].end()) {
                    continue;
                }
                std::string text;
                const auto &chain = arm->second;
                for(std::size_t leg = 0; leg < chain.size(); ++leg) {
                    if(chain[leg].first && leg + 1 < chain.size()) {
                        text += renderer.render(chain[leg].first) + " ?";
                        text += renderer.render(chain[leg].second) + " :";
                    } else {
                        text += renderer.render(chain[leg].second);
                    }
                }
                decode.values.push_back({state_names[i], text});
            }
            process.decodes.push_back(decode);
        }
        for(std::size_t i = 0; i < states.size(); ++i) {
            for(const auto &transition : states[i].out) {
                FsmReport::Transition edge;
                edge.from = state_names[i];
                edge.to = state_names[transition.next];
                if(transition.guard) {
                    edge.guard = renderer.render(transition.guard);
                }
                std::vector<std::string> updates;
                for(const auto &stmt : *transition.action) {
                    auto text = renderer.render(stmt);
                    while(!text.empty() && (text.back() == ';' || std::isspace(text.back()))) {
                        text.pop_back();
                    }
                    updates.push_back(text);
                }
                edge.action = Misc::StringUtils::join("; ", updates);
                process.transitions.push_back(edge);
            }
        }
        m_report->processes.push_back(process);
    }

    return result;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
