// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <cctype>
#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

bool has_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
{
    const auto &pragmas = pragmalist->get_pragmas();
    if(!pragmas) {
        return false;
    }
    for(const auto &pragma : *pragmas) {
        if(pragma && pragma->get_name() == name) {
            return true;
        }
    }
    return false;
}

AST::Pragma::Ptr get_pragma(const AST::Pragmalist::Ptr &pragmalist, const std::string &name)
{
    const auto &pragmas = pragmalist->get_pragmas();
    if(!pragmas) {
        return nullptr;
    }
    for(const auto &pragma : *pragmas) {
        if(pragma && pragma->get_name() == name) {
            return pragma;
        }
    }
    return nullptr;
}

bool contains_event_statement(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::EventStatement)) {
        return true;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        if(contains_event_statement(child)) {
            return true;
        }
    }
    return false;
}

void collect_identifier_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        names.insert(AST::cast_to<AST::Identifier>(node)->get_name());
        return;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        collect_identifier_names(child, names);
    }
}

/// The target register of a straight-line nonblocking assignment: a plain
/// identifier, or null when the shape is outside the subset.
std::string nba_target(const AST::NonblockingSubstitution::Ptr &nba)
{
    const auto &lvalue = nba->get_left();
    if(!lvalue || !lvalue->get_var()) {
        return "";
    }
    if(!lvalue->get_var()->is_node_type(AST::NodeType::Identifier)) {
        return "";
    }
    return AST::cast_to<AST::Identifier>(lvalue->get_var())->get_name();
}

std::string to_lower(const std::string &str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

unsigned int clog2(unsigned int value)
{
    unsigned int width = 0;
    for(unsigned int remaining = value - 1; remaining != 0; remaining >>= 1) {
        ++width;
    }
    return width == 0 ? 1 : width;
}

AST::IntConstN::Ptr make_const(unsigned int value, int size, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::IntConstN>(fn, ln);
    node->set_base(10);
    node->set_size(size);
    node->set_sign(false);
    node->set_value(value);
    return node;
}

AST::Identifier::Ptr make_id(const std::string &name, const std::string &fn, int ln)
{
    auto node = std::make_shared<AST::Identifier>(fn, ln);
    node->set_name(name);
    return node;
}

AST::Dimension::ListPtr make_packed_range(unsigned int msb, const std::string &fn, int ln)
{
    auto range = std::make_shared<AST::RangeDim>(fn, ln);
    range->set_left(AST::to_node(make_const(msb, -1, fn, ln)));
    range->set_right(AST::to_node(make_const(0, -1, fn, ln)));
    auto dims = std::make_shared<AST::Dimension::List>();
    dims->push_back(range);
    return dims;
}

AST::NonblockingSubstitution::Ptr make_state_assign(const std::string &state_reg,
                                                    const std::string &state_name,
                                                    const std::string &fn, int ln)
{
    auto lvalue = std::make_shared<AST::Lvalue>(fn, ln);
    lvalue->set_var(AST::to_node(make_id(state_reg, fn, ln)));
    auto rvalue = std::make_shared<AST::Rvalue>(fn, ln);
    rvalue->set_var(AST::to_node(make_id(state_name, fn, ln)));
    auto nba = std::make_shared<AST::NonblockingSubstitution>(fn, ln);
    nba->set_left(lvalue);
    nba->set_right(rvalue);
    return nba;
}

} // namespace

int ImplicitFsmElaboration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    if(!node->is_node_type(AST::NodeType::Module)) {
        return recurse_in_childs(node);
    }

    const auto &module = AST::cast_to<AST::Module>(node);
    const auto &items = module->get_items();
    if(!items) {
        return 0;
    }

    // Marked processes are collected first: replacing an item invalidates
    // iteration, and the count decides the prefix — several processes get an
    // ordinal each, one shared prefix would collide by construction (§10).
    std::vector<std::pair<AST::Pragmalist::Ptr, AST::Initial::Ptr>> marked;
    int ret = 0;

    for(const auto &item : *items) {
        if(!item || !item->is_node_type(AST::NodeType::Pragmalist)) {
            continue;
        }
        const auto &pragmalist = AST::cast_to<AST::Pragmalist>(item);
        if(!has_pragma(pragmalist, "veriparse_fsm")) {
            continue;
        }
        const auto &statements = pragmalist->get_statements();
        if(!statements) {
            continue;
        }
        for(const auto &stmt : *statements) {
            if(stmt->is_node_type(AST::NodeType::Initial)) {
                marked.emplace_back(pragmalist, AST::cast_to<AST::Initial>(stmt));
            } else if(stmt->is_node_category(AST::NodeType::Always)) {
                if(contains_event_statement(AST::cast_to<AST::Always>(stmt)->get_statement())) {
                    LOG_ERROR_N(stmt) << "(* veriparse_fsm *) on an always process holding "
                                      << "event controls: not compiled — rewrite the body as "
                                      << "'initial forever begin ... end' and drop the senslist";
                    ret += 1;
                } else {
                    LOG_WARNING_N(stmt)
                        << "(* veriparse_fsm *) has no effect: this process is already RTL";
                }
            } else {
                LOG_ERROR_N(stmt) << "(* veriparse_fsm *) on an item that is not a "
                                  << "process: there is nothing to compile";
                ret += 1;
            }
        }
    }

    if(ret) {
        return ret;
    }

    std::size_t ordinal = 0;
    for(const auto &elt : marked) {
        const std::string prefix =
            (marked.size() > 1) ? ("__fsm" + std::to_string(ordinal)) : "__fsm";
        ret += compile_process(module, node, elt.first, elt.second, prefix);
        ++ordinal;
    }

    return ret;
}

int ImplicitFsmElaboration::flatten_body(const AST::Node::Ptr &node, AST::Node::ListPtr atoms)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &statements = AST::cast_to<AST::Block>(node)->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                if(flatten_body(stmt, atoms)) {
                    return 1;
                }
            }
        }
        return 0;
    }

    case AST::NodeType::EventStatement: {
        const auto &event = AST::cast_to<AST::EventStatement>(node);
        atoms->push_back(node);
        // `@(posedge clk) stmt;` attaches the statement to the wait: it runs
        // after the edge, so it belongs to the following segment.
        return flatten_body(event->get_statement(), atoms);
    }

    case AST::NodeType::NonblockingSubstitution:
        atoms->push_back(node);
        return 0;

    case AST::NodeType::BlockingSubstitution:
        LOG_ERROR_N(node) << "blocking assignment in a marked process: '=' names a "
                          << "combinational value the lowering does not handle yet "
                          << "— a register takes '<=' (ADR-0014 §6)";
        return 1;

    case AST::NodeType::DelayStatement:
        LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                          << "with no hardware meaning (ADR-0014 §9)";
        return 1;

    case AST::NodeType::SystemCall:
        LOG_ERROR_N(node) << "system task in a marked process: no hardware "
                          << "meaning — the mark landed on testbench code? (ADR-0014 §9)";
        return 1;

    case AST::NodeType::WaitStatement:
        LOG_ERROR_N(node) << "level-sensitive wait in a marked process: not an "
                          << "edge, no boundary to cut at (ADR-0014 §9)";
        return 1;

    default:
        LOG_ERROR_N(node) << "construct not handled by the FSM lowering: " << node->get_node_type();
        return 1;
    }
}

int ImplicitFsmElaboration::check_wait(const AST::EventStatement::Ptr &event, AST::Sens::Ptr &clock)
{
    const auto &senslist = event->get_senslist();
    if(!senslist || !senslist->get_list() || senslist->get_list()->size() != 1) {
        LOG_ERROR_N(event) << "a marked process waits on exactly one event "
                           << "term: one clock, one edge (ADR-0014 §2)";
        return 1;
    }

    const auto &sens = senslist->get_list()->front();
    if(sens->get_type() != AST::Sens::TypeEnum::POSEDGE &&
       sens->get_type() != AST::Sens::TypeEnum::NEGEDGE) {
        LOG_ERROR_N(event) << "a marked process waits on a clock edge — "
                           << "posedge or negedge, never a level (ADR-0014 §2)";
        return 1;
    }

    if(!sens->get_sig() || !sens->get_sig()->is_node_type(AST::NodeType::Identifier)) {
        LOG_ERROR_N(event) << "a marked process waits on a plain clock signal";
        return 1;
    }

    if(!clock) {
        clock = sens;
        return 0;
    }

    const auto &clock_name = AST::cast_to<AST::Identifier>(clock->get_sig())->get_name();
    const auto &sens_name = AST::cast_to<AST::Identifier>(sens->get_sig())->get_name();
    if(clock_name != sens_name || clock->get_type() != sens->get_type()) {
        LOG_ERROR_N(event) << "cut point over '" << sens_name
                           << "' disagrees with the process clock '" << clock_name
                           << "': one clock, one edge, per process (ADR-0014 §2, §9)";
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
    // disagree with (§5.3).
    if(!bare.empty()) {
        const bool bare_odd = bare.size() <= qualified.size();
        const auto &odd = bare_odd ? bare : qualified;
        const auto &rest = bare_odd ? qualified : bare;
        std::string odd_lines, rest_lines;
        for(const auto &wait : odd) {
            odd_lines += (odd_lines.empty() ? "" : ", ") + std::to_string(wait->get_line());
        }
        for(const auto &wait : rest) {
            rest_lines += (rest_lines.empty() ? "" : ", ") + std::to_string(wait->get_line());
        }
        LOG_ERROR_N(odd.front())
            << "the wait" << (odd.size() > 1 ? "s" : "") << " at line"
            << (odd.size() > 1 ? "s " : " ") << odd_lines << (bare_odd ? " carry no" : " carry an")
            << " `iff` while the waits at line" << (rest.size() > 1 ? "s " : " ") << rest_lines
            << (bare_odd ? " carry one" : " carry none")
            << ": a chip enable qualifies every transition or none (ADR-0014 §5.3)";
        return 1;
    }

    const auto &reference = qualified.front()->get_senslist()->get_list()->front()->get_condition();
    for(const auto &wait : qualified) {
        const auto &condition = wait->get_senslist()->get_list()->front()->get_condition();
        if(!reference->is_equal(condition, false)) {
            LOG_ERROR_N(wait)
                << "the wait at line " << wait->get_line() << " carries a different `iff` "
                << "condition than the wait at line " << qualified.front()->get_line()
                << ": a uniform chip enable is one condition for the whole machine — "
                << "gating states differently is a separate feature (ADR-0014 §5.3, §15)";
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
                                    << "as a string or an identifier (ADR-0014 §5)";
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
                                << "(* veriparse_reset = \"<port>\" *) (ADR-0014 §5)";
            return 1;
        }
        reset_name = matches.front();
    }

    const auto &lowered = to_lower(reset_name);
    active_low = lowered.size() >= 2 && lowered.compare(lowered.size() - 2, 2, "_n") == 0;
    return 0;
}

int ImplicitFsmElaboration::check_segments(const AST::Node::ListPtr &init_stmts,
                                           const std::vector<Segment> &segments)
{
    // Registers of the process: every nonblocking target anywhere in it.
    std::set<std::string> process_regs;
    auto collect_targets = [&process_regs](const AST::Node::ListPtr &stmts) -> int {
        for(const auto &stmt : *stmts) {
            if(!stmt->is_node_type(AST::NodeType::NonblockingSubstitution)) {
                continue;
            }
            const auto &target = nba_target(AST::cast_to<AST::NonblockingSubstitution>(stmt));
            if(target.empty()) {
                LOG_ERROR_N(stmt) << "nonblocking target is not a plain register "
                                  << "identifier: not handled by the lowering yet";
                return 1;
            }
            process_regs.insert(target);
        }
        return 0;
    };
    if(collect_targets(init_stmts)) {
        return 1;
    }
    for(const auto &segment : segments) {
        if(collect_targets(segment.statements)) {
            return 1;
        }
    }

    // One commit per register per segment (§6), and no register read before
    // a path out of reset has written it (§5.1, §6). Reads resolve against
    // the entry store, so a segment's own writes do not define its reads.
    std::set<std::string> defined;
    auto check_one = [&](const AST::Node::ListPtr &stmts, bool is_init) -> int {
        std::set<std::string> written;
        for(const auto &stmt : *stmts) {
            const auto &nba = AST::cast_to<AST::NonblockingSubstitution>(stmt);
            const auto &target = nba_target(nba);

            if(!is_init) {
                std::set<std::string> reads;
                collect_identifier_names(AST::to_node(nba->get_right()), reads);
                for(const auto &read : reads) {
                    if(process_regs.count(read) && !defined.count(read)) {
                        LOG_ERROR_N(stmt) << "register '" << read << "' is read before any path "
                                          << "out of reset assigns it, and the init segment gives "
                                          << "it no value (ADR-0014 §5.1, §6)";
                        return 1;
                    }
                }
            }

            if(!written.insert(target).second) {
                LOG_ERROR_N(stmt) << "register '" << target << "' committed twice on "
                                  << "one path: the first '<=' never takes effect "
                                  << "(ADR-0014 §6)";
                return 1;
            }
        }
        defined.insert(written.begin(), written.end());
        return 0;
    };

    if(check_one(init_stmts, true)) {
        return 1;
    }
    for(const auto &segment : segments) {
        if(check_one(segment.statements, false)) {
            return 1;
        }
    }
    return 0;
}

int ImplicitFsmElaboration::compile_process(const AST::Module::Ptr &module,
                                            const AST::Node::Ptr &parent,
                                            const AST::Pragmalist::Ptr &pragmalist,
                                            const AST::Initial::Ptr &initial,
                                            const std::string &prefix)
{
    const auto &atoms = std::make_shared<AST::Node::List>();
    if(flatten_body(initial->get_statement(), atoms)) {
        return 1;
    }

    // Cut at every wait (§4): statements before the first wait are the reset
    // branch, each run between waits is one state's action.
    AST::Sens::Ptr clock;
    const auto &init_stmts = std::make_shared<AST::Node::List>();
    std::vector<Segment> segments;
    std::vector<AST::EventStatement::Ptr> waits;
    AST::Node::ListPtr current = init_stmts;

    for(const auto &atom : *atoms) {
        if(atom->is_node_type(AST::NodeType::EventStatement)) {
            const auto &event = AST::cast_to<AST::EventStatement>(atom);
            if(check_wait(event, clock)) {
                return 1;
            }
            waits.push_back(event);
            segments.push_back(Segment{std::make_shared<AST::Node::List>()});
            current = segments.back().statements;
        } else {
            current->push_back(atom);
        }
    }

    if(!clock) {
        LOG_ERROR_N(initial) << "(* veriparse_fsm *) on an initial with no wait: "
                             << "there is nothing to compile (ADR-0014 §2, §9)";
        return 1;
    }

    AST::Node::Ptr enable;
    if(check_enable(waits, enable)) {
        return 1;
    }

    std::string reset_name;
    bool active_low = false;
    if(find_reset(module, pragmalist, reset_name, active_low)) {
        return 1;
    }

    if(check_segments(init_stmts, segments)) {
        return 1;
    }

    const auto &emitted =
        emit(module, clock, enable, reset_name, active_low, init_stmts, segments, prefix);
    if(!emitted) {
        return 1;
    }

    AST::Node::Ptr pragmalist_node = AST::to_node(pragmalist);
    AST::Node::Ptr parent_node = parent;
    pickup_statements(parent_node, pragmalist_node, emitted);
    return 0;
}

AST::Node::ListPtr
ImplicitFsmElaboration::emit(const AST::Module::Ptr &module, const AST::Sens::Ptr &clock,
                             const AST::Node::Ptr &enable, const std::string &reset_name,
                             bool active_low, const AST::Node::ListPtr &init_stmts,
                             const std::vector<Segment> &segments, const std::string &prefix)
{
    const std::string &fn = module->get_filename();
    const int ln = module->get_line();

    // One state per segment after a wait, plus the hold state a one-shot
    // parks in (§2).
    const std::size_t nstates = segments.size() + 1;
    const unsigned int width = clog2(static_cast<unsigned int>(nstates));

    std::vector<std::string> state_names;
    for(std::size_t i = 0; i < segments.size(); ++i) {
        state_names.push_back(prefix + "_state_" + std::to_string(i));
    }
    state_names.push_back(prefix + "_hold");
    const std::string state_reg = prefix + "_state";

    // §10: a collision remaining after prefixing is an error, not a rename.
    Analysis::UniqueDeclaration::IdentifierSet declared;
    Analysis::UniqueDeclaration::analyze(AST::to_node(module), declared);
    for(const auto &name : state_names) {
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << name
                                << "' collides with an existing one (ADR-0014 §10)";
            return nullptr;
        }
    }
    if(Analysis::UniqueDeclaration::identifier_declaration_exists(state_reg, declared)) {
        LOG_ERROR_N(module) << "generated declaration '" << state_reg
                            << "' collides with an existing one (ADR-0014 §10)";
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
        param->set_value(AST::to_node(
            make_const(static_cast<unsigned int>(i), static_cast<int>(width), fn, ln)));
        result->push_back(AST::to_node(param));
    }

    // logic [w-1:0] <prefix>_state;
    auto reg_type = std::make_shared<AST::LogicType>(fn, ln);
    reg_type->set_packed_dims(make_packed_range(width - 1, fn, ln));
    auto reg = std::make_shared<AST::Var>(fn, ln);
    reg->set_name(state_reg);
    reg->set_type(reg_type);
    result->push_back(AST::to_node(reg));

    // Reset branch: the init segment verbatim, plus the state register
    // going to the first state (§5.1).
    const auto &reset_stmts = std::make_shared<AST::Node::List>();
    for(const auto &stmt : *init_stmts) {
        reset_stmts->push_back(stmt->clone());
    }
    reset_stmts->push_back(AST::to_node(make_state_assign(state_reg, state_names[0], fn, ln)));
    auto reset_block = std::make_shared<AST::Block>(reset_stmts, "", fn, ln);

    // case (<state>) arms: each segment's statements plus the transition;
    // the hold state carries no update and re-enters itself by staying put.
    const auto &caselist = std::make_shared<AST::Case::List>();
    for(std::size_t i = 0; i < segments.size(); ++i) {
        const auto &arm_stmts = std::make_shared<AST::Node::List>();
        for(const auto &stmt : *segments[i].statements) {
            arm_stmts->push_back(stmt->clone());
        }
        arm_stmts->push_back(
            AST::to_node(make_state_assign(state_reg, state_names[i + 1], fn, ln)));

        const auto &conds = std::make_shared<AST::Node::List>();
        conds->push_back(AST::to_node(make_id(state_names[i], fn, ln)));
        auto arm = std::make_shared<AST::Case>(fn, ln);
        arm->set_cond(conds);
        arm->set_statement(AST::to_node(std::make_shared<AST::Block>(arm_stmts, "", fn, ln)));
        caselist->push_back(arm);
    }
    {
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

    auto always = std::make_shared<AST::AlwaysFF>(fn, ln);
    always->set_senslist(std::make_shared<AST::Senslist>(sens_list, fn, ln));
    always->set_statement(AST::to_node(guard));
    result->push_back(AST::to_node(always));

    return result;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
