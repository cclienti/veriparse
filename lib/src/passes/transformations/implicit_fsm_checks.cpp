// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "implicit_fsm_detail.hpp"

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace FsmDetail
{

int check_called_functions(const AST::Module::Ptr &module, const AST::Initial::Ptr &initial)
{
    const auto &calls = Analysis::Module::get_functioncall_nodes(AST::to_node(initial));
    if(!calls || calls->empty()) {
        return 0;
    }
    Analysis::Module::FunctionMap functions;
    Analysis::Module::get_function_dictionary(AST::to_node(module), functions);

    // Worklist over the call graph: a function is only as pure as what it
    // calls, so callees queue behind their callers.
    std::vector<std::string> worklist;
    std::set<std::string> checked;
    for(const auto &call : *calls) {
        worklist.push_back(call->get_name());
    }
    while(!worklist.empty()) {
        const auto name = worklist.back();
        worklist.pop_back();
        if(!checked.insert(name).second) {
            continue;
        }
        const auto &found = functions.find(name);
        if(found == functions.end()) {
            continue;
        }
        const auto &function = found->second;
        std::set<std::string> locals = {function->get_name()};
        std::set<std::string> written_locals;
        const auto &args = function->get_args();
        std::set<std::string> aliases;
        if(args) {
            for(const auto &arg : *args) {
                const auto direction = arg->get_direction();
                if(direction == AST::Arg::DirectionEnum::REF ||
                   direction == AST::Arg::DirectionEnum::CONST_REF) {
                    // A reference formal reads its actual live — purity
                    // admits the read; a write through it is the side
                    // effect the model would miss, checked below.
                    if(function->get_lifetime() != AST::Function::LifetimeEnum::AUTOMATIC) {
                        LOG_ERROR_N(function) << "function '" << name << "': a ref argument needs "
                                              << "'function automatic' (IEEE 1800-2017 §13.5.2)";
                        return 1;
                    }
                    aliases.insert(arg->get_name());
                    locals.insert(arg->get_name());
                    continue;
                }
                if(direction != AST::Arg::DirectionEnum::INPUT &&
                   direction != AST::Arg::DirectionEnum::NONE) {
                    LOG_ERROR_N(function)
                        << "function '" << name << "' carries a non-input argument: a "
                        << "side effect in expression position the (R_p, s_p) model "
                        << "would miss silently";
                    return 1;
                }
                locals.insert(arg->get_name());
            }
        }
        std::set<std::string> targets;
        const auto &statements = function->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                // The impurity may hide in the body itself: a system call,
                // or a further function this worklist will visit.
                if(check_impure_calls(stmt)) {
                    return 1;
                }
                collect_declaration_names(stmt, locals);
                collect_driven(stmt, targets);
            }
        }
        const auto &nested = Analysis::Module::get_functioncall_nodes(AST::to_node(function));
        if(nested) {
            for(const auto &sub : *nested) {
                worklist.push_back(sub->get_name());
            }
        }
        for(const auto &target : targets) {
            if(aliases.count(target)) {
                LOG_ERROR_N(function)
                    << "function '" << name << "' writes through reference argument '" << target
                    << "': a side effect in expression position the "
                    << "(R_p, s_p) model would miss silently — and a 'const ref' may "
                    << "not be written at all (IEEE 1800-2017 §13.5.2)";
                return 1;
            }
            if(!locals.count(target)) {
                LOG_ERROR_N(function)
                    << "function '" << name << "' writes non-local '" << target
                    << "': a side effect in expression position the (R_p, s_p) model "
                    << "would miss silently — pure functions are accepted";
                return 1;
            }
            if(target != function->get_name()) {
                written_locals.insert(target);
            }
        }
        // A static-lifetime function writing a local keeps state across
        // calls: successive evaluations differ, which breaks the walk's
        // stable-read assumption. The return variable and the arguments
        // are re-assigned per call and carry nothing observable.
        if(function->get_lifetime() != AST::Function::LifetimeEnum::AUTOMATIC) {
            for(const auto &target : written_locals) {
                bool is_arg = false;
                if(args) {
                    for(const auto &arg : *args) {
                        is_arg |= arg->get_name() == target;
                    }
                }
                if(!is_arg) {
                    LOG_ERROR_N(function)
                        << "function '" << name << "' has static lifetime and writes "
                        << "its local '" << target << "': the local keeps its value "
                        << "across calls, so successive evaluations differ — declare "
                        << "the function automatic";
                    return 1;
                }
            }
        }
    }

    // §13.5.2 on the process's own call sites: a reference formal must bind
    // a variable actual, never a net. Only the process-level calls resolve
    // against module scope; a call nested in a function body binds that
    // function's locals, which the loop above already vetted.
    for(const auto &call : *calls) {
        const auto &found = functions.find(call->get_name());
        if(found == functions.end() || !found->second->get_args() || !call->get_args()) {
            continue;
        }
        auto actual_it = call->get_args()->begin();
        for(const auto &arg : *found->second->get_args()) {
            if(actual_it == call->get_args()->end()) {
                break;
            }
            const AST::Node::Ptr actual = *actual_it;
            ++actual_it;
            if(arg->get_direction() != AST::Arg::DirectionEnum::REF &&
               arg->get_direction() != AST::Arg::DirectionEnum::CONST_REF) {
                continue;
            }
            AST::Identifier::Ptr base;
            if(actual && actual->is_node_type(AST::NodeType::Identifier)) {
                base = AST::cast_to<AST::Identifier>(actual);
            } else {
                base = select_base(actual);
            }
            if(!base) {
                continue;
            }
            const std::string &aname = base->get_name();
            if(is_net_signal(module, aname)) {
                log_net_actual(AST::to_node(call), "function", call->get_name(), arg->get_name(),
                               aname);
                return 1;
            }
        }
    }
    return 0;
}

/// Whether two conditions are structural complements: X against !X on
/// either side, the walk's own Eq/NotEq pairs over equal operands, or two
/// equalities pinning one expression to different folded constants.

} // namespace FsmDetail

using namespace FsmDetail;

namespace
{

// Helpers of this unit alone, moved out of the shared header.

/// Registers assigned on EVERY runtime path through the statement run: a
/// sequence unions, a branch keeps only what all its arms agree on — an
/// `if` with no `else`, or a `case` with no `default`, guarantees nothing.
void must_writes(const AST::Node::Ptr &node, std::set<std::string> &writes);
void must_writes_list(const AST::Node::ListPtr &stmts, std::set<std::string> &writes)
{
    for(const auto &stmt : *stmts) {
        must_writes(stmt, writes);
    }
}
void must_writes(const AST::Node::Ptr &node, std::set<std::string> &writes)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                must_writes(stmt, writes);
            }
        }
        break;
    }
    case AST::NodeType::NonblockingSubstitution:
        writes.insert(nba_target(AST::cast_to<AST::NonblockingSubstitution>(node)));
        break;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        if(!ifs->get_false_statement()) {
            break;
        }
        std::set<std::string> true_writes, false_writes;
        must_writes(ifs->get_true_statement(), true_writes);
        must_writes(ifs->get_false_statement(), false_writes);
        for(const auto &name : true_writes) {
            if(false_writes.count(name)) {
                writes.insert(name);
            }
        }
        break;
    }
    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        if(!caselist || caselist->empty()) {
            break;
        }
        bool has_default = false;
        bool first = true;
        std::set<std::string> agreed;
        for(const auto &arm : *caselist) {
            const auto &conds = arm->get_cond();
            if(!conds || conds->empty()) {
                has_default = true;
            }
            std::set<std::string> arm_writes;
            must_writes(arm->get_statement(), arm_writes);
            if(first) {
                agreed = arm_writes;
                first = false;
            } else {
                std::set<std::string> kept;
                for(const auto &name : agreed) {
                    if(arm_writes.count(name)) {
                        kept.insert(name);
                    }
                }
                agreed = kept;
            }
        }
        if(has_default) {
            writes.insert(agreed.begin(), agreed.end());
        }
        break;
    }
    default:
        break;
    }
}

} // namespace

int ImplicitFsmElaboration::check_wait(const AST::EventStatement::Ptr &event, AST::Sens::Ptr &clock)
{
    const auto &senslist = event->get_senslist();
    if(!senslist || !senslist->get_list() || senslist->get_list()->size() != 1) {
        LOG_ERROR_N(event) << "a marked process waits on exactly one event "
                           << "term: one clock, one edge";
        return 1;
    }

    const auto &sens = senslist->get_list()->front();
    if(sens->get_type() != AST::Sens::TypeEnum::POSEDGE &&
       sens->get_type() != AST::Sens::TypeEnum::NEGEDGE) {
        LOG_ERROR_N(event) << "a marked process waits on a clock edge — "
                           << "posedge or negedge, never a level";
        return 1;
    }

    if(!sens->get_sig() || !sens->get_sig()->is_node_type(AST::NodeType::Identifier)) {
        LOG_ERROR_N(event) << "a marked process waits on a plain clock signal";
        return 1;
    }

    // The `iff` enable is read at every state's entry (§5.3): the least
    // stable place of all for an impure call.
    if(check_impure_calls(sens->get_condition())) {
        return 1;
    }

    if(!clock) {
        clock = sens;
        return 0;
    }

    const auto &clock_name =
        Analysis::Statement::identifier_key(AST::cast_to<AST::Identifier>(clock->get_sig()));
    const auto &sens_name =
        Analysis::Statement::identifier_key(AST::cast_to<AST::Identifier>(sens->get_sig()));
    if(clock_name != sens_name || clock->get_type() != sens->get_type()) {
        LOG_ERROR_N(event) << "cut point over '" << sens_name
                           << "' disagrees with the process clock '" << clock_name
                           << "': one clock, one edge, per process";
        return 1;
    }

    return 0;
}

int ImplicitFsmElaboration::check_enable(const std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Node::Ptr &enable)
{
    std::vector<AST::EventStatement::Ptr> bare;
    std::vector<AST::EventStatement::Ptr> qualified;
    for(const auto &wait : waits) {
        const auto &sens = wait->get_senslist()->get_list()->front();
        if(sens->get_condition()) {
            qualified.push_back(wait);
        } else {
            bare.push_back(wait);
        }
    }

    if(qualified.empty()) {
        enable = nullptr;
        return 0;
    }

    // Some waits qualified and some bare is almost always an oversight, and
    // it is not repairable by guessing: adding or dropping an enable changes
    // the machine. The message names the odd waits out and the ones they
    // disagree with (§5.3). Lines are deduplicated: waits replicated by
    // unrolling share one source line and are one wait to the author.
    if(!bare.empty()) {
        const bool bare_odd = bare.size() <= qualified.size();
        const auto &odd = bare_odd ? bare : qualified;
        const auto &rest = bare_odd ? qualified : bare;
        auto lines_of = [](const std::vector<AST::EventStatement::Ptr> &group) {
            std::set<int> lines;
            for(const auto &wait : group) {
                lines.insert(wait->get_line());
            }
            std::vector<std::string> strs;
            for(int line : lines) {
                strs.push_back(std::to_string(line));
            }
            return std::make_pair(lines.size(), Misc::StringUtils::join(", ", strs));
        };
        const auto &odd_lines = lines_of(odd);
        const auto &rest_lines = lines_of(rest);
        auto describe = [](const std::pair<std::size_t, std::string> &lines) {
            return (lines.first > 1 ? "the waits at lines " : "the wait at line ") + lines.second;
        };
        LOG_ERROR_N(odd.front()) << describe(odd_lines)
                                 << (odd_lines.first > 1 ? " carry" : " carries")
                                 << (bare_odd ? " no `iff`" : " an `iff`") << " while "
                                 << describe(rest_lines)
                                 << (rest_lines.first > 1 ? " carry" : " carries")
                                 << (bare_odd ? " one" : " none")
                                 << ": a chip enable qualifies every transition or none";
        return 1;
    }

    const auto &reference = qualified.front()->get_senslist()->get_list()->front()->get_condition();
    for(std::size_t i = 1; i < qualified.size(); ++i) {
        const auto &wait = qualified[i];
        const auto &condition = wait->get_senslist()->get_list()->front()->get_condition();
        if(!reference->is_equal(condition, false)) {
            LOG_ERROR_N(wait) << "the wait at line " << wait->get_line()
                              << " carries a different `iff` "
                              << "condition than the wait at line " << qualified.front()->get_line()
                              << ": a uniform chip enable is one condition for the whole machine — "
                              << "gating states differently is a separate feature";
            return 1;
        }
    }

    enable = reference;
    return 0;
}

int ImplicitFsmElaboration::find_reset(const AST::Module::Ptr &module,
                                       const AST::Pragmalist::Ptr &pragmalist,
                                       std::string &reset_name, bool &active_low)
{
    const auto &hint = get_pragma(pragmalist, "veriparse_reset");
    if(hint) {
        const auto &expr = hint->get_expression();
        if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
            reset_name = AST::cast_to<AST::StringConst>(expr)->get_value();
        } else if(expr && expr->is_node_type(AST::NodeType::Identifier)) {
            reset_name = AST::cast_to<AST::Identifier>(expr)->get_name();
        } else {
            LOG_ERROR_N(pragmalist) << "veriparse_reset names the reset input, "
                                    << "as a string or an identifier";
            return 1;
        }
    } else {
        static const std::set<std::string> candidates = {"rst_n", "resetn", "aresetn", "rst",
                                                         "reset"};
        std::vector<std::string> matches;
        const auto &inputs = Analysis::Module::get_input_names(AST::to_node(module));
        for(const auto &name : inputs) {
            if(candidates.count(to_lower(name))) {
                matches.push_back(name);
            }
        }
        if(matches.size() != 1) {
            LOG_ERROR_N(module) << "reset signal neither hinted nor uniquely "
                                << "inferable: " << matches.size()
                                << " candidate input(s) — name it with "
                                << "(* veriparse_reset = \"<port>\" *)";
            return 1;
        }
        reset_name = matches.front();
    }

    const auto &lowered = to_lower(reset_name);
    active_low = lowered.size() >= 2 && lowered.compare(lowered.size() - 2, 2, "_n") == 0;

    // §3: veriparse_reset_level overrides the suffix inference,
    // veriparse_reset_kind picks the always_ff's reset flavour.
    const auto &level = get_pragma(pragmalist, "veriparse_reset_level");
    if(level) {
        mpz_class value;
        if(!level->get_expression() ||
           !ExpressionEvaluation().evaluate_node(level->get_expression(), value) ||
           (value != 0 && value != 1)) {
            LOG_ERROR_N(pragmalist) << "veriparse_reset_level is 0 or 1";
            return 1;
        }
        active_low = value == 0;
    }
    const auto &kind = get_pragma(pragmalist, "veriparse_reset_kind");
    if(kind) {
        const auto &expr = kind->get_expression();
        std::string wanted;
        if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
            wanted = AST::cast_to<AST::StringConst>(expr)->get_value();
        }
        if(wanted != "sync" && wanted != "async") {
            LOG_ERROR_N(pragmalist) << "veriparse_reset_kind is \"sync\" or \"async\"";
            return 1;
        }
        m_proc.async_reset = wanted == "async";
    }
    return 0;
}

int ImplicitFsmElaboration::check_paths(const AST::Node::ListPtr &init_stmts,
                                        const std::vector<State> &states, std::size_t entry_next,
                                        const AST::Node::Ptr &enable)
{
    // Registers of the process: every nonblocking target anywhere in it,
    // the branches an action keeps verbatim included.
    std::set<std::string> process_regs;
    if(collect_targets_list(init_stmts, process_regs)) {
        return 1;
    }
    for(const auto &state : states) {
        for(const auto &transition : state.out) {
            if(collect_targets_list(transition.action, process_regs)) {
                return 1;
            }
        }
    }

    // At most one commit per register on any runtime path through an action
    // (§6) — a branch counts its worst arm, a sequence adds. The error is
    // anchored at the last '<=' to the offending register, the one whose
    // predecessor never takes effect.
    auto commits_ok = [](const AST::Node::ListPtr &stmts) -> int {
        std::map<std::string, int> counts;
        max_commits_list(stmts, counts);
        for(const auto &elt : counts) {
            if(elt.second > 1) {
                AST::Node::Ptr anchor = stmts->front();
                std::function<void(const AST::Node::Ptr &)> find = [&](const AST::Node::Ptr &node) {
                    if(!node) {
                        return;
                    }
                    if(node->is_node_type(AST::NodeType::NonblockingSubstitution)) {
                        if(nba_target(AST::cast_to<AST::NonblockingSubstitution>(node)) ==
                           elt.first) {
                            anchor = node;
                        }
                        return;
                    }
                    for(const auto &child : *node->get_children()) {
                        find(child);
                    }
                };
                for(const auto &stmt : *stmts) {
                    find(stmt);
                }
                LOG_ERROR_N(anchor)
                    << "register '" << elt.first << "' committed twice on one path: "
                    << "the first '<=' never takes effect";
                return 1;
            }
        }
        return 0;
    };
    if(!init_stmts->empty() && commits_ok(init_stmts)) {
        return 1;
    }

    // §5.1: the reset branch loads reset values once. Emitted under
    // `if (!rst)` a preamble branch would be re-evaluated on every reset
    // cycle — the source initial evaluates it exactly once — and an arm
    // that skips a register leaves it with no reset value.
    for(const auto &stmt : *init_stmts) {
        if(stmt->is_node_type(AST::NodeType::IfStatement) ||
           stmt->is_node_type(AST::NodeType::CaseStatement)) {
            LOG_ERROR_N(stmt) << "a branch in the preamble: the reset branch loads reset "
                              << "values once, while emitted into the reset arm it would be "
                              << "re-evaluated on every reset cycle";
            return 1;
        }
    }

    // A preamble read of a process register is a read of the empty entry
    // store: nothing is assigned at reset entry — its own '<=' commits only
    // at the clock edge (§5.1, §6).
    for(const auto &stmt : *init_stmts) {
        std::set<std::string> reads;
        collect_reads(stmt, reads);
        for(const auto &read : reads) {
            if(process_regs.count(read)) {
                LOG_ERROR_N(stmt) << "the preamble reads register '" << read
                                  << "': nothing is assigned at reset entry, so the reset "
                                  << "value would be undefined";
                return 1;
            }
        }
    }

    // Must-defined registers at each state's entry: what the init segment
    // wrote, then the intersection over every incoming path — a register a
    // branch arm skips is not defined past the merge. Transitions only go
    // forward in source order (no loops yet), so one pass in state order
    // sees every predecessor first.
    std::set<std::string> init_defined;
    must_writes_list(init_stmts, init_defined);

    // The enable is read at every state's entry, the first out of reset
    // included, so a process register it reads must come up with a value
    // the init segment supplies (§5.1, §5.3, §6).
    if(enable) {
        std::set<std::string> reads;
        collect_identifier_names(enable, reads);
        for(const auto &read : reads) {
            if(process_regs.count(read) && !init_defined.count(read)) {
                LOG_ERROR_N(enable)
                    << "the `iff` enable reads register '" << read << "' which no path "
                    << "out of reset assigns: the enable gates every state including "
                    << "the first, and the init segment gives it no value";
                return 1;
            }
        }
    }

    // Back-edges (§7.3) make the state graph cyclic, so the must-defined
    // sets iterate to a fixpoint: they only ever shrink once seeded, so
    // the iteration terminates. A transition's write set never changes:
    // computed once, the fixpoint is pure set arithmetic.
    std::vector<std::vector<std::set<std::string>>> writes_of(states.size());
    for(std::size_t s = 0; s < states.size(); ++s) {
        for(const auto &transition : states[s].out) {
            writes_of[s].emplace_back();
            must_writes_list(transition.action, writes_of[s].back());
        }
    }
    std::vector<std::set<std::string>> defined_in(states.size());
    std::vector<bool> reached(states.size(), false);
    if(entry_next < states.size()) {
        defined_in[entry_next] = init_defined;
        reached[entry_next] = true;
    }
    bool changed = true;
    while(changed) {
        changed = false;
        for(std::size_t s = 0; s < states.size(); ++s) {
            if(!reached[s]) {
                continue;
            }
            for(std::size_t t = 0; t < states[s].out.size(); ++t) {
                const auto &transition = states[s].out[t];
                if(transition.next >= states.size()) {
                    continue;
                }
                std::set<std::string> defined_out = defined_in[s];
                defined_out.insert(writes_of[s][t].begin(), writes_of[s][t].end());
                if(!reached[transition.next]) {
                    defined_in[transition.next] = defined_out;
                    reached[transition.next] = true;
                    changed = true;
                    continue;
                }
                std::set<std::string> kept;
                for(const auto &name : defined_in[transition.next]) {
                    if(defined_out.count(name)) {
                        kept.insert(name);
                    }
                }
                if(kept.size() != defined_in[transition.next].size()) {
                    defined_in[transition.next] = kept;
                    changed = true;
                }
            }
        }
    }

    for(std::size_t s = 0; s < states.size(); ++s) {
        if(!reached[s]) {
            continue;
        }
        for(const auto &transition : states[s].out) {
            if(!transition.action->empty() && commits_ok(transition.action)) {
                return 1;
            }

            // Reads resolve against the entry store (§6.1): the action's own
            // writes commit at the next edge and define nothing here. The
            // guard is a read too — the fork asks the question at entry.
            // Each error is anchored at the statement or guard that reads.
            auto check_reads = [&](const std::set<std::string> &reads,
                                   const AST::Node::Ptr &anchor) -> int {
                for(const auto &read : reads) {
                    if(process_regs.count(read) && !defined_in[s].count(read)) {
                        LOG_ERROR_N(anchor)
                            << "register '" << read << "' is read before every path "
                            << "out of reset assigns it, and the init segment gives "
                            << "it no value";
                        return 1;
                    }
                }
                return 0;
            };
            if(transition.guard) {
                std::set<std::string> reads;
                collect_identifier_names(transition.guard, reads);
                if(check_reads(reads, transition.guard)) {
                    return 1;
                }
            }
            for(const auto &stmt : *transition.action) {
                std::set<std::string> reads;
                collect_reads(stmt, reads);
                if(check_reads(reads, stmt)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/// Every name driven outside @p pragmalist: other processes and continuous
/// assigns — their block-locals subtracted, their called tasks' writes
/// included — and instance output/inout connections when the instantiated
/// definition is in @p modules (a black box is skipped). One walk shared by
/// the §9.2.2.4 conflict check and the §6.2 stability scan, so neither is
/// ever the weaker of the two.
void collect_foreign_drivers(const AST::Module::Ptr &module, const AST::Pragmalist::Ptr &pragmalist,
                             const Analysis::Module::ModulesMap *modules,
                             std::set<std::string> &others)
{
    const auto &tasks = Analysis::Module::get_task_nodes(AST::to_node(module));
    const auto &items = module->get_items();
    if(items) {
        for(const auto &item : *items) {
            if(item.get() == static_cast<AST::Node *>(pragmalist.get())) {
                continue;
            }
            // An instance drives whatever its output and inout ports
            // connect to — visible when the instantiated module's
            // definition is in the map, skipped as a black box when not.
            if(item->is_node_type(AST::NodeType::Instancelist) && modules) {
                const auto &instancelist = AST::cast_to<AST::Instancelist>(item);
                const auto &definition = modules->find(instancelist->get_module());
                const auto &instances = instancelist->get_instances();
                if(definition == modules->end() || !instances) {
                    continue;
                }
                const auto &def_ports = definition->second->get_ports();
                for(const auto &instance : *instances) {
                    const auto &connections = instance->get_portlist();
                    if(!connections || !def_ports) {
                        continue;
                    }
                    std::size_t position = 0;
                    for(const auto &connection : *connections) {
                        // Resolve the connected port's NAME — given, or
                        // positional through the header — then its
                        // direction, which a non-ANSI module states in
                        // its body, not its header.
                        std::string port_name = connection->get_name();
                        if(port_name.empty() && position < def_ports->size()) {
                            auto it_port = def_ports->begin();
                            std::advance(it_port, position);
                            port_name = header_port_name(*it_port);
                        }
                        ++position;
                        const auto direction = child_port_direction(definition->second, port_name);
                        if(direction == AST::Port::DirectionEnum::OUTPUT ||
                           direction == AST::Port::DirectionEnum::INOUT) {
                            collect_lvalue_bases(connection->get_value(), others);
                        }
                    }
                }
                continue;
            }
            if(!item->is_node_type(AST::NodeType::Always) &&
               !item->is_node_type(AST::NodeType::AlwaysFF) &&
               !item->is_node_type(AST::NodeType::AlwaysComb) &&
               !item->is_node_type(AST::NodeType::AlwaysLatch) &&
               !item->is_node_type(AST::NodeType::Initial) &&
               !item->is_node_type(AST::NodeType::Assign) &&
               !item->is_node_type(AST::NodeType::Pragmalist) &&
               !item->is_node_type(AST::NodeType::GenerateStatement)) {
                continue;
            }
            // Per item: what it drives, minus what it declares — a
            // block-local shadowing the register's name is its own
            // variable. A task it calls writes on its behalf.
            std::set<std::string> driven, declared, called;
            collect_driven(item, driven);
            collect_declaration_names(item, declared);
            collect_call_names(item, called);
            if(tasks) {
                for(const auto &task : *tasks) {
                    if(!called.count(task->get_name())) {
                        continue;
                    }
                    const auto &statements = task->get_statements();
                    if(statements) {
                        for(const auto &stmt : *statements) {
                            collect_driven(stmt, driven);
                            collect_declaration_names(stmt, declared);
                        }
                    }
                    const auto &targs = task->get_args();
                    if(targs) {
                        for(const auto &arg : *targs) {
                            declared.insert(arg->get_name());
                        }
                    }
                }
            }
            for(const auto &name : driven) {
                if(!declared.count(name)) {
                    others.insert(name);
                }
            }
        }
    }
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
