// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/transformations/fsm_task_inliner.hpp>
#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <cctype>
#include <functional>
#include <map>
#include <set>
#include "implicit_fsm_detail.hpp"

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

using namespace FsmDetail;

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

    for(const auto &pair : collect_marked(module)) {
        const auto &pragmalist = pair.first;
        const auto &stmt = pair.second;
        {
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

    // §7.4: every task call of the marked processes inlines first — its
    // own pass, so the structural pre-lowering is runnable and testable
    // on its own; the induced commits arrive as pragma markers this
    // pass adopts per process. Running it here keeps every driver and
    // caller working unchanged, and a pipeline that already ran it costs
    // one no-op.
    if(FsmTaskInliner().run(node)) {
        return 1;
    }

    // §3: veriparse_prefix overrides the default; several processes
    // without hints get an ordinal each. Every prefix must be distinct —
    // a hint that collides with another process's hint or ordinal is
    // rejected here, where the clash can be named, not later at the §10
    // declaration check where the message would blame the wrong thing.
    std::vector<std::string> prefixes;
    for(std::size_t i = 0; i < marked.size(); ++i) {
        std::string prefix = (marked.size() > 1) ? ("__fsm" + std::to_string(i)) : "__fsm";
        const auto &prefix_hint = get_pragma(marked[i].first, "veriparse_prefix");
        if(prefix_hint) {
            const auto &expr = prefix_hint->get_expression();
            std::string wanted;
            if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
                wanted = AST::cast_to<AST::StringConst>(expr)->get_value();
            } else if(expr && expr->is_node_type(AST::NodeType::Identifier)) {
                wanted = AST::cast_to<AST::Identifier>(expr)->get_name();
            }
            bool valid =
                !wanted.empty() &&
                (std::isalpha(static_cast<unsigned char>(wanted.front())) || wanted.front() == '_');
            for(const char c : wanted) {
                valid &= std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
            }
            if(!valid) {
                LOG_ERROR_N(marked[i].first) << "veriparse_prefix wants an identifier to prefix "
                                             << "the generated declarations with";
                return 1;
            }
            prefix = wanted;
        }
        prefixes.push_back(prefix);
    }
    for(std::size_t i = 0; i < prefixes.size(); ++i) {
        for(std::size_t j = i + 1; j < prefixes.size(); ++j) {
            if(prefixes[i] == prefixes[j]) {
                LOG_ERROR_N(marked[j].first)
                    << "prefix '" << prefixes[j] << "' is already taken by another "
                    << "marked process of this module — hint or ordinal — and the "
                    << "generated declarations need distinct prefixes";
                return 1;
            }
        }
    }
    for(std::size_t i = 0; i < marked.size(); ++i) {
        ret += compile_process(module, node, marked[i].first, marked[i].second, prefixes[i]);
    }

    return ret;
}

int ImplicitFsmElaboration::collect_body(const AST::Node::Ptr &node,
                                         std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Sens::Ptr &clock, bool &has_wait, TempScope scope)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &statements = AST::cast_to<AST::Block>(node)->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                // §6: a declaration met here names a temporary — legal only
                // in a scope no cut point spans, visible to the block's
                // later statements, dead at its end.
                if(stmt->is_node_type(AST::NodeType::Var)) {
                    const auto &var = AST::cast_to<AST::Var>(stmt);
                    if(var->get_init() &&
                       check_impure_calls(AST::to_node(var->get_init()->get_var()))) {
                        return 1;
                    }
                    if(contains_event_statement(node)) {
                        LOG_ERROR_N(stmt)
                            << "temporary '" << var->get_name() << "' is declared in a "
                            << "scope a cut point spans: it would have to outlive the "
                            << "cycle, which '=' cannot mean — a register takes '<=' at "
                            << "module level";
                        return 1;
                    }
                    if(var->get_unpacked_dims() && !var->get_unpacked_dims()->empty()) {
                        LOG_ERROR_N(stmt) << "array temporary '" << var->get_name()
                                          << "': not handled by the lowering yet";
                        return 1;
                    }
                    if(scope.visible.count(var->get_name())) {
                        LOG_ERROR_N(stmt)
                            << "temporary '" << var->get_name() << "' shadows one of an "
                            << "enclosing scope: substitution binds by name — rename it";
                        return 1;
                    }
                    scope.visible.insert(var->get_name());
                    scope.writable.insert(var->get_name());
                    if(!scope.verbatim) {
                        m_proc.temps[var->get_name()] = var;
                    }
                    continue;
                }
                if(collect_body(stmt, waits, clock, has_wait, scope)) {
                    return 1;
                }
            }
        }
        return 0;
    }

    case AST::NodeType::EventStatement: {
        const auto &event = AST::cast_to<AST::EventStatement>(node);
        if(check_wait(event, clock)) {
            return 1;
        }
        m_proc.wait_index[event.get()] = waits.size();
        waits.push_back(event);
        has_wait = true;
        // `@(posedge clk) stmt;` attaches the statement to the wait: it runs
        // after the edge, so it belongs to the following segment.
        return collect_body(event->get_statement(), waits, clock, has_wait, scope);
    }

    case AST::NodeType::NonblockingSubstitution: {
        const auto &nba = AST::cast_to<AST::NonblockingSubstitution>(node);
        // `#d q <= e` and `q <= #d e` park the delay on the assignment
        // itself, not on a DelayStatement.
        if(nba->get_ldelay() || nba->get_rdelay()) {
            LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                              << "with no hardware meaning";
            return 1;
        }
        // §6.2: one signal, one discipline — the set makes the check
        // order-independent against the '=' classification.
        m_proc.nba_targets.insert(nba_target(nba));
        if(m_proc.decoded.count(nba_target(nba))) {
            LOG_ERROR_N(node) << "'<=' to '" << nba_target(nba) << "', which already takes "
                              << "'=': a target cannot be a register and a decoded output "
                              << "at once";
            return 1;
        }
        // §6.1 consequence 3: one variable cannot be both a wire and a
        // flop — '<=' to a temporary is a contradiction, not a choice.
        if(scope.visible.count(nba_target(nba))) {
            LOG_ERROR_N(node) << "'<=' to temporary '" << nba_target(nba)
                              << "': it already takes '=' — mixing the two forms on one "
                              << "target contradicts what the variable is";
            return 1;
        }
        // Both sides: the left holds index expressions.
        if(check_impure_calls(AST::to_node(nba->get_left()))) {
            return 1;
        }
        return check_impure_calls(AST::to_node(nba->get_right()));
    }

    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        // §6.1: a write under a branch to an outer temporary would make its
        // value conditional — arms may only write what they declare. A
        // cut-point-free branch stays verbatim, and its own temporaries are
        // ordinary scoped SystemVerilog there, not walked ones.
        TempScope arm_scope = scope;
        arm_scope.writable.clear();
        arm_scope.verbatim =
            scope.verbatim || !(contains_event_statement(node) || contains_jump(node) ||
                                contains_outer_blocking(node));
        bool arms_wait = false;
        if(collect_body(ifs->get_true_statement(), waits, clock, arms_wait, arm_scope)) {
            return 1;
        }
        if(collect_body(ifs->get_false_statement(), waits, clock, arms_wait, arm_scope)) {
            return 1;
        }
        // Rejected in condition position wherever it appears: a verbatim
        // branch would carry the call into the always_ff (§9).
        if(check_impure_calls(ifs->get_cond())) {
            return 1;
        }
        if(arms_wait || contains_jump(node) || contains_outer_blocking(node)) {
            m_proc.forking.insert(node.get());
        }
        has_wait |= arms_wait;
        return 0;
    }

    case AST::NodeType::CaseStatement: {
        const auto &caselist = AST::cast_to<AST::CaseStatement>(node)->get_caselist();
        TempScope arm_scope = scope;
        arm_scope.writable.clear();
        arm_scope.verbatim =
            scope.verbatim || !(contains_event_statement(node) || contains_jump(node) ||
                                contains_outer_blocking(node));
        bool arms_wait = false;
        std::size_t defaults = 0;
        if(caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(!conds || conds->empty()) {
                    ++defaults;
                }
                if(collect_body(arm->get_statement(), waits, clock, arms_wait, arm_scope)) {
                    return 1;
                }
            }
        }
        // The grammar admits several default arms; IEEE 1800-2017 §12.5
        // allows at most one, and the guard construction has no condition
        // to give a second one.
        if(defaults > 1) {
            LOG_ERROR_N(node) << "case with " << defaults << " default arms in a marked "
                              << "process: at most one (IEEE 1800-2017 §12.5)";
            return 1;
        }
        // A forking case is if-converted with `==`, but item matching is
        // case equality (IEEE 1800-2017 §12.5): an item with x/z bits can
        // never satisfy the guard, so it is rejected, not silently dropped.
        // A cut-point-free case stays verbatim and keeps its semantics.
        const bool forking = arms_wait || contains_jump(node) || contains_outer_blocking(node);
        if(forking && caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(!conds) {
                    continue;
                }
                for(const auto &value : *conds) {
                    if(!value->is_node_type(AST::NodeType::IntConst)) {
                        continue;
                    }
                    const auto &text = AST::cast_to<AST::IntConst>(value)->get_value();
                    if(text.find_first_of("xXzZ?") != std::string::npos) {
                        LOG_ERROR_N(value) << "case item '" << text << "' with x/z bits in a case "
                                           << "holding cut points: the fork guard uses logical "
                                           << "equality, which such an item never satisfies "
                                           << "(IEEE 1800-2017 §12.5)";
                        return 1;
                    }
                }
            }
        }
        if(check_impure_calls(AST::cast_to<AST::CaseStatement>(node)->get_comp())) {
            return 1;
        }
        if(forking) {
            m_proc.forking.insert(node.get());
        }
        has_wait |= arms_wait;
        return 0;
    }

    case AST::NodeType::CasexStatement:
    case AST::NodeType::CasezStatement:
        LOG_ERROR_N(node) << "casex/casez in a marked process: wildcard matching "
                          << "is not handled by the lowering yet";
        return 1;

    case AST::NodeType::Pragmalist: {
        // (* veriparse_no_unroll *) directly on a loop keeps it rolled
        // (§7.2); any other statement pragma is transparent.
        const auto &pragmalist = AST::cast_to<AST::Pragmalist>(node);
        const bool no_unroll = has_pragma(pragmalist, "veriparse_no_unroll");
        const auto &statements = pragmalist->get_statements();
        if(statements) {
            for(const auto &stmt : *statements) {
                const bool loop = stmt->is_node_type(AST::NodeType::RepeatStatement) ||
                                  stmt->is_node_type(AST::NodeType::ForStatement) ||
                                  stmt->is_node_type(AST::NodeType::WhileStatement);
                if(no_unroll && loop) {
                    if(collect_loop(stmt, true, waits, clock, has_wait)) {
                        return 1;
                    }
                } else if(collect_body(stmt, waits, clock, has_wait, scope)) {
                    return 1;
                }
            }
        }
        return 0;
    }

    case AST::NodeType::WhileStatement:
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::RepeatStatement:
    case AST::NodeType::ForStatement:
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::ForeverStatement:
        // §2: the perpetual form — the §7.3 back-edge with no exit test.
        return collect_loop(node, false, waits, clock, has_wait);

    case AST::NodeType::Break:
    case AST::NodeType::Continue:
        // §8: a CFG edge — resolved during the path walk, where the
        // innermost enclosing loop is known.
        return 0;

    case AST::NodeType::ParallelBlock:
        LOG_ERROR_N(node) << "fork/join in a marked process: concurrent control "
                          << "flow the state model cannot express (IEEE 1800-2017 §9.3.2)";
        return 1;

    case AST::NodeType::Disable:
        LOG_ERROR_N(node) << "disable in a marked process: abortive control flow "
                          << "the state model cannot express (IEEE 1800-2017 §9.6.2)";
        return 1;

    case AST::NodeType::TaskCall:
    case AST::NodeType::Call:
        LOG_ERROR_N(node) << "statement call to '"
                          << AST::cast_to<AST::Identifier>(node)->get_name()
                          << "': no task by that name in the module — a known task "
                          << "would have been inlined (§7.4) — and a function called "
                          << "as a statement discards its result";
        return 1;

    case AST::NodeType::SingleStatement: {
        const auto &single = AST::cast_to<AST::SingleStatement>(node);
        if(single->get_delay()) {
            LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                              << "with no hardware meaning";
            return 1;
        }
        return collect_body(single->get_statement(), waits, clock, has_wait, scope);
    }

    case AST::NodeType::BlockingSubstitution: {
        // §6: '=' names a combinational value within one cycle, and the
        // check is scoping — its target is a temporary declared in a
        // scope no cut point spans, written where it is declared.
        const auto &blocking = AST::cast_to<AST::BlockingSubstitution>(node);
        const auto &target = lvalue_target(blocking->get_left());
        if(blocking->get_ldelay() || blocking->get_rdelay()) {
            LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                              << "with no hardware meaning";
            return 1;
        }
        if(check_impure_calls(AST::to_node(blocking->get_right()))) {
            return 1;
        }
        if(scope.writable.count(target)) {
            return 0;
        }
        if(target.empty()) {
            LOG_ERROR_N(node) << "'=' target is not a plain identifier: slice writes to "
                              << "a temporary are not handled by the lowering yet";
            return 1;
        }
        if(scope.visible.count(target)) {
            LOG_ERROR_N(node) << "'=' to temporary '" << target << "' under a branch it is not "
                              << "declared in: the value would be conditional, which v1 does not "
                              << "if-convert — declare the temporary inside the branch, or assign "
                              << "it once before it";
            return 1;
        }
        {
            // §6.2: a module-level '=' target is a decoded output.
            bool is_input = false;
            bool is_port = false;
            const auto &mdecl = find_declaration(m_proc.module, target, &is_input, &is_port);
            if(mdecl) {
                if(is_input) {
                    LOG_ERROR_N(node) << "'=' to input port '" << target
                                      << "': no process may drive it (IEEE §9.2.2.4)";
                    return 1;
                }
                // A decoded output is a variable the emitted always_comb
                // drives. Before DefaultResolution an ANSI `output logic`
                // is still an ImplicitNet carrying its data type, and
                // §23.2.2.3 makes that port a variable; an explicit net
                // keyword is a net whatever data type it carries, and a
                // name that declares no signal at all — a parameter, a
                // typedef — has nothing to drive.
                const auto &as_net = std::dynamic_pointer_cast<AST::Net>(mdecl);
                const bool port_variable =
                    is_port && mdecl->is_node_type(AST::NodeType::ImplicitNet) && as_net &&
                    as_net->get_type() &&
                    !as_net->get_type()->is_node_type(AST::NodeType::ImplicitType);
                if(as_net && !port_variable) {
                    LOG_ERROR_N(node)
                        << "'=' to '" << target << "', which is a net: a decoded output "
                        << "is a variable the emitted always_comb drives (IEEE 1800-2017 "
                           "§23.2.2.3)";
                    return 1;
                }
                if(!as_net && !std::dynamic_pointer_cast<AST::Var>(mdecl)) {
                    LOG_ERROR_N(node)
                        << "'=' to '" << target << "', which is not a variable: nothing "
                        << "for a decoded output to drive";
                    return 1;
                }
                if(m_proc.nba_targets.count(target)) {
                    LOG_ERROR_N(node)
                        << "'=' to '" << target << "', which already takes '<=': a target "
                        << "cannot be a register and a decoded output at once";
                    return 1;
                }
                m_proc.decoded[target] = AST::to_node(mdecl);
                return 0;
            }
        }
        LOG_ERROR_N(node) << "'=' to '" << target << "', which is not a scope-local "
                          << "temporary: a value that must survive the edge takes '<=', "
                          << "a temporary is declared in a scope without a cut point";
        return 1;
    }

    case AST::NodeType::DelayStatement:
        LOG_ERROR_N(node) << "'#' delay in a marked process: simulation timing "
                          << "with no hardware meaning";
        return 1;

    case AST::NodeType::SystemCall:
        LOG_ERROR_N(node) << "system task in a marked process: no hardware "
                          << "meaning — the mark landed on testbench code?";
        return 1;

    case AST::NodeType::WaitStatement:
        LOG_ERROR_N(node) << "level-sensitive wait in a marked process: not an "
                          << "edge, no boundary to cut at";
        return 1;

    default:
        LOG_ERROR_N(node) << "construct not handled by the FSM lowering: " << node->get_node_type();
        return 1;
    }
}

int ImplicitFsmElaboration::collect_loop(const AST::Node::Ptr &node, bool kept_rolled,
                                         std::vector<AST::EventStatement::Ptr> &waits,
                                         AST::Sens::Ptr &clock, bool &has_wait)
{
    LoopInfo info;
    switch(node->get_node_type()) {
    case AST::NodeType::WhileStatement: {
        const auto &loop = AST::cast_to<AST::WhileStatement>(node);
        info.kind = LoopInfo::Kind::WHILE;
        info.cond = loop->get_cond();
        info.body = loop->get_statement();
        break;
    }
    case AST::NodeType::ForeverStatement: {
        info.kind = LoopInfo::Kind::FOREVER;
        info.body = AST::cast_to<AST::ForeverStatement>(node)->get_statement();
        break;
    }
    case AST::NodeType::RepeatStatement: {
        const auto &loop = AST::cast_to<AST::RepeatStatement>(node);
        info.kind = LoopInfo::Kind::REPEAT;
        info.cond = loop->get_times();
        info.body = loop->get_statement();
        mpz_class value;
        if(ExpressionEvaluation().evaluate_node(info.cond, value)) {
            // §12.7.2 gives x/z a meaning (zero) but a negative count none,
            // and tools disagree on one: almost always a parameterization
            // off-by-N, so it is rejected rather than silently guessed.
            if(value < 0) {
                LOG_ERROR_N(info.cond)
                    << "repeat count folds to " << value << ": a loop cannot execute "
                    << "a negative number of times — a parameterization off-by-N? "
                    << "(IEEE 1800-2017 §12.7.2)";
                return 1;
            }
            if(value > mpz_class(0xFFFFFFFFUL)) {
                LOG_ERROR_N(info.cond)
                    << "repeat count folds to " << value << ": beyond any countdown "
                    << "the lowering will size";
                return 1;
            }
            info.count_known = true;
            info.count_value = value.convert_to<unsigned long>();
        }
        break;
    }
    case AST::NodeType::ForStatement: {
        const auto &loop = AST::cast_to<AST::ForStatement>(node);
        info.kind = LoopInfo::Kind::FOR;
        info.cond = loop->get_cond();
        info.body = loop->get_statement();
        const auto &pre = loop->get_pre();
        const auto &post = loop->get_post();
        if(!pre || !post || !info.cond) {
            LOG_ERROR_N(node) << "a rolled for honours the construct's full contract and "
                              << "needs all three of init, test and step";
            return 1;
        }
        const auto &pre_target = nba_like_target(pre);
        const auto &post_target = nba_like_target(post);
        if(pre_target.empty() || pre_target != post_target) {
            LOG_ERROR_N(node) << "a rolled for's init and step assign the same plain index "
                              << "register";
            return 1;
        }
        info.index = pre_target;
        info.init_rhs = pre->get_right() ? pre->get_right()->get_var() : nullptr;
        info.step_rhs = post->get_right() ? post->get_right()->get_var() : nullptr;
        if(!info.init_rhs || !info.step_rhs) {
            LOG_ERROR_N(node) << "a rolled for's init and step carry plain expressions";
            return 1;
        }
        break;
    }
    default:
        return 1;
    }

    // Loop conditions — and a rolled for's init/step, which the walk
    // substitutes and duplicates — must read stably in their segment.
    if(check_impure_calls(info.cond) || check_impure_calls(info.init_rhs) ||
       check_impure_calls(info.step_rhs)) {
        return 1;
    }

    // §15 gives each repeat-nesting depth its own shared countdown:
    // sequential repeats at a depth re-initialise on entry and share its
    // register; a nested counting repeat uses the next depth's, so its
    // reload leaves the outer count alone. A repeat that induces no
    // counter (a folded count of 0 or 1) consumes no depth.
    const bool counting_repeat =
        info.kind == LoopInfo::Kind::REPEAT && (!info.count_known || info.count_value >= 2);
    if(counting_repeat) {
        info.depth = m_proc.repeat_depth;
        ++m_proc.repeat_depth;
    }
    bool body_wait = false;
    const int body_rc = collect_body(info.body, waits, clock, body_wait, TempScope{});
    if(counting_repeat) {
        --m_proc.repeat_depth;
    }
    if(body_rc) {
        return 1;
    }

    if(!body_wait) {
        if(kept_rolled) {
            LOG_ERROR_N(node) << "(* veriparse_no_unroll *) on a loop without a cut point: "
                              << "the loop runs in zero time and there is no state to save — "
                              << "drop the hint and let it unroll";
        } else {
            LOG_ERROR_N(node) << "loop with no cut point survived to the FSM lowering: no "
                              << "static exit, or the unroller refused it — a zero-delay "
                              << "loop has no hardware meaning (IEEE 1800-2017 §9.2.2.1)";
        }
        return 1;
    }

    // §7.2/§8: a bounded loop the unroller left behind has a correct rolled
    // lowering, but rolled is opt-in — a bound that stopped folding or a
    // refused jump shape must not change the state count in silence.
    if(!kept_rolled && info.kind != LoopInfo::Kind::WHILE && info.kind != LoopInfo::Kind::FOREVER) {
        LOG_ERROR_N(node) << "bounded loop with a cut point was not unrolled upstream "
                          << "(non-constant bound, or a jump shape the unroller refuses): "
                          << "mark it (* veriparse_no_unroll *) to compile it rolled, or "
                          << "make the bound constant";
        return 1;
    }

    m_proc.loops[node.get()] = info;
    has_wait = true;
    return 0;
}

void ImplicitFsmElaboration::push_frame(std::vector<Frame> &frames, const AST::Node::Ptr &node,
                                        const AST::Node *loop)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Block)) {
        const auto &block = AST::cast_to<AST::Block>(node);
        const auto &stmts = block->get_statements();
        if(stmts) {
            std::vector<AST::Var::Ptr> decls;
            for(const auto &stmt : *stmts) {
                if(stmt->is_node_type(AST::NodeType::Var)) {
                    decls.push_back(AST::cast_to<AST::Var>(stmt));
                }
            }
            frames.push_back(Frame{stmts, stmts->begin(), loop, block->get_scope(), decls});
        }
        return;
    }
    if(node->is_node_type(AST::NodeType::Pragmalist)) {
        const auto &stmts = AST::cast_to<AST::Pragmalist>(node)->get_statements();
        if(stmts) {
            frames.push_back(Frame{stmts, stmts->begin(), loop, "", {}});
        }
        return;
    }
    auto single = std::make_shared<AST::Node::List>();
    single->push_back(node);
    frames.push_back(Frame{single, single->begin(), loop, "", {}});
}

/// The §10.1 label composition in force at a walk position: the frames'
/// labels joined outward-in.
std::string ImplicitFsmElaboration::labels_of(const std::vector<Frame> &frames)
{
    std::string stem;
    for(const auto &frame : frames) {
        if(frame.label.empty()) {
            continue;
        }
        stem += stem.empty() ? frame.label : "_" + frame.label;
    }
    return stem;
}

int ImplicitFsmElaboration::walk_paths(std::size_t from, const AST::Node::Ptr &guard,
                                       AST::Node::ListPtr action, std::vector<Frame> frames,
                                       Env env, std::set<const AST::Node *> lapped,
                                       std::vector<State> &states, std::vector<Transition> &entry)
{
    const auto record = [&](std::size_t next) -> int {
        if(next >= states.size()) {
            m_proc.hold_needed = true;
        }
        // §6.2 totality: every path assigns every decoded output — a skip
        // is where the source holds and the emitted comb tracks.
        std::map<std::string, AST::Node::Ptr> decode;
        for(const auto &elt : m_proc.decoded) {
            const auto &it = env.find(elt.first);
            if(it == env.end()) {
                LOG_ERROR_N(elt.second)
                    << "decoded output '" << elt.first << "' is not assigned on a path "
                    << (from == k_entry ? std::string("of the init segment")
                                        : "leaving the state at line " +
                                              std::to_string(states[from].wait->get_line()))
                    << ": every path between two cut points assigns every decoded "
                    << "output";
                return 1;
            }
            decode[elt.first] = it->second;
        }
        if(from == k_entry) {
            entry.push_back(Transition{guard, action, next, decode});
        } else {
            states[from].out.push_back(Transition{guard, action, next, decode});
        }
        return 0;
    };

    while(!frames.empty()) {
        Frame &top = frames.back();
        if(top.it == top.stmts->end()) {
            // §6: a block's temporaries die at its end — the scope is the
            // lifetime, and a read past it must not substitute.
            for(const auto &decl : top.decls) {
                env.erase(decl->get_name());
            }
            // The end of a loop's body is its back-edge (§7.3): fork at the
            // loop head again instead of popping through.
            if(top.loop) {
                const AST::Node *loop = top.loop;
                frames.pop_back();
                return loop_fork(loop, false, from, guard, action, frames, env, lapped, states,
                                 entry);
            }
            frames.pop_back();
            continue;
        }
        const AST::Node::Ptr stmt = *top.it;
        ++top.it;

        const std::string &fn = stmt->get_filename();
        const int ln = stmt->get_line();

        switch(stmt->get_node_type()) {
        case AST::NodeType::Block: {
            const auto &label = AST::cast_to<AST::Block>(stmt)->get_scope();
            if(!label.empty() && !contains_event_statement(stmt)) {
                if(from == k_entry) {
                    // §10.1: the init segment is not a state — a label whose
                    // block holds no cut point has nothing to point at. One
                    // that does names its states through the stack instead.
                    LOG_WARNING_N(stmt)
                        << "label '" << label << "' names the init segment: the reset "
                        << "branch is not a state, so the name is dropped";
                } else if(guard) {
                    // Under a fork guard the block does not delimit the
                    // state alone: naming it after one arm would be wrong on
                    // the paths that take the other (§10.1).
                    LOG_WARNING_N(stmt)
                        << "label '" << label << "' sits under a fork condition: it does "
                        << "not delimit a state alone, so the name is dropped";
                } else if(states[from].stem.empty()) {
                    // A cut-point-free labelled block is one state's action:
                    // the state it belongs to takes the name (§10.1).
                    const auto &outer = labels_of(frames);
                    states[from].stem = outer.empty() ? label : outer + "_" + label;
                }
            }
            push_frame(frames, stmt);
            break;
        }

        case AST::NodeType::Pragmalist:
            push_frame(frames, stmt);
            break;

        case AST::NodeType::SingleStatement:
            // Validated delay-free by the collector; the wrapper is
            // transparent.
            push_frame(frames, AST::cast_to<AST::SingleStatement>(stmt)->get_statement());
            break;

        case AST::NodeType::Var: {
            // §6: a temporary's declaration — its frame carries the scope,
            // the environment carries the value, and an initializer is the
            // first assignment.
            const auto &var = AST::cast_to<AST::Var>(stmt);
            if(var->get_init() && var->get_init()->get_var()) {
                if(process_blocking(var, var->get_name(), AST::to_node(var->get_init()->get_var()),
                                    env, fn, ln)) {
                    return 1;
                }
            }
            break;
        }

        case AST::NodeType::BlockingSubstitution: {
            // §6.1: substitution, not rewriting — the value dissolves into
            // its readers through the environment (process_blocking).
            const auto &blocking = AST::cast_to<AST::BlockingSubstitution>(stmt);
            const auto &target = lvalue_target(blocking->get_left());
            if(m_proc.decoded.count(target)) {
                // §6.2: a decoded output's segment value — substituted for
                // later readers like any blocking value; the path's end
                // snapshots it as the arrival value.
                const auto &value =
                    clone_subst(AST::to_node(blocking->get_right()->get_var()), env);
                if(check_temp_reads(value, env)) {
                    return 1;
                }
                env[target] = value;
                break;
            }
            const auto &decl = find_temp_decl(frames, target);
            if(!decl) {
                LOG_ERROR_N(stmt) << "'=' to '" << target << "' with no declaration in "
                                  << "scope: the collector should have refused this — "
                                  << "please report this input";
                return 1;
            }
            if(process_blocking(decl, target, AST::to_node(blocking->get_right()->get_var()), env,
                                fn, ln)) {
                return 1;
            }
            break;
        }

        case AST::NodeType::NonblockingSubstitution: {
            // §7.4: a copy-in capture — and a copy-out, whose measured
            // §13.3 visibility is immediate — is an induced commit that
            // forward-substitutes within its own segment (§6.1): a reader
            // before the first cut sees the value, later segments the
            // register.
            if(m_inline.captures.count(stmt.get()) || m_inline.copyouts.count(stmt.get())) {
                const auto &nba = AST::cast_to<AST::NonblockingSubstitution>(stmt);
                const auto &value = clone_subst(AST::to_node(nba->get_right()->get_var()), env);
                if(check_temp_reads(value, env)) {
                    return 1;
                }
                push_induced(action, nba_target(nba), value, fn, ln);
                env[nba_target(nba)] = value;
                break;
            }
            const auto &placed = env.empty() ? stmt : subst_into(stmt->clone(), env);
            if(check_temp_reads(placed, env)) {
                return 1;
            }
            // A user commit supersedes an induced capture to the same
            // target in this segment: the capture's value already reached
            // its readers through the environment, and storage takes the
            // last write, as in the source (§6.1, §7.4).
            {
                const std::string &target =
                    nba_target(AST::cast_to<AST::NonblockingSubstitution>(placed));
                for(auto it = action->begin(); it != action->end();) {
                    const bool induced =
                        m_proc.induced.count(it->get()) &&
                        (*it)->is_node_type(AST::NodeType::NonblockingSubstitution) &&
                        nba_target(AST::cast_to<AST::NonblockingSubstitution>(*it)) == target;
                    it = induced ? action->erase(it) : std::next(it);
                }
            }
            action->push_back(placed);
            break;
        }

        case AST::NodeType::IfStatement: {
            // Cut-point-free: a plain conditional inside the action, no
            // state spent on it (§4). One holding an inlined call carries
            // its induced commits through the branch-local environment.
            if(!m_proc.forking.count(stmt.get())) {
                if(contains_induced(stmt)) {
                    std::set<std::string> committed;
                    Env env_branch = env;
                    const auto &placed = emit_verbatim(stmt, env_branch, committed);
                    if(!placed) {
                        return 1;
                    }
                    for(const auto &target : committed) {
                        env[target] = nullptr; // branch-dependent past the if
                    }
                    action->push_back(placed);
                    break;
                }
                const auto &placed = env.empty() ? stmt : subst_into(stmt->clone(), env);
                if(check_temp_reads(placed, env)) {
                    return 1;
                }
                action->push_back(placed);
                break;
            }
            const auto &ifs = AST::cast_to<AST::IfStatement>(stmt);
            {
                bool dead = false;
                const auto &cond_subst = clone_subst(ifs->get_cond(), env);
                if(check_temp_reads(cond_subst, env)) {
                    return 1;
                }
                const auto &leg_guard = conjoin(guard, cond_subst, fn, ln, &dead);
                if(!dead) {
                    warn_fork_arm_label(ifs->get_true_statement());
                    std::vector<Frame> leg = frames;
                    push_frame(leg, ifs->get_true_statement());
                    if(walk_paths(from, leg_guard, copy_list(action), leg, env, lapped, states,
                                  entry)) {
                        return 1;
                    }
                }
            }
            {
                // A missing else is the fall-through path: the machine takes
                // it in zero extra statements, not zero probability.
                bool dead = false;
                const auto &leg_guard = conjoin(
                    guard, make_ulnot(clone_subst(ifs->get_cond(), env), fn, ln), fn, ln, &dead);
                if(!dead) {
                    warn_fork_arm_label(ifs->get_false_statement());
                    std::vector<Frame> leg = frames;
                    push_frame(leg, ifs->get_false_statement());
                    if(walk_paths(from, leg_guard, copy_list(action), leg, env, lapped, states,
                                  entry)) {
                        return 1;
                    }
                }
            }
            return 0;
        }

        case AST::NodeType::CaseStatement: {
            if(!m_proc.forking.count(stmt.get())) {
                if(contains_induced(stmt)) {
                    std::set<std::string> committed;
                    Env env_branch = env;
                    const auto &placed = emit_verbatim(stmt, env_branch, committed);
                    if(!placed) {
                        return 1;
                    }
                    for(const auto &target : committed) {
                        env[target] = nullptr; // arm-dependent past the case
                    }
                    action->push_back(placed);
                    break;
                }
                const auto &placed = env.empty() ? stmt : subst_into(stmt->clone(), env);
                if(check_temp_reads(placed, env)) {
                    return 1;
                }
                action->push_back(placed);
                break;
            }
            const auto &cs = AST::cast_to<AST::CaseStatement>(stmt);
            // One leg per arm, guarded by the disjunction of its matches;
            // the default arm — or the fall-through when there is none —
            // takes the conjunction of every match negated.
            std::vector<std::pair<AST::Node::Ptr, AST::Node::Ptr>> legs;
            AST::Node::Ptr not_any;
            bool has_default = false;
            const auto &caselist = cs->get_caselist();
            if(caselist) {
                for(const auto &arm : *caselist) {
                    const auto &conds = arm->get_cond();
                    if(!conds || conds->empty()) {
                        has_default = true;
                        legs.emplace_back(nullptr, arm->get_statement());
                        continue;
                    }
                    AST::Node::Ptr match;
                    for(const auto &value : *conds) {
                        const auto &comp_subst = clone_subst(cs->get_comp(), env);
                        if(check_temp_reads(comp_subst, env)) {
                            return 1;
                        }
                        const auto &eq = make_eq(comp_subst, value->clone(), fn, ln);
                        match = match ? make_lor(match, eq, fn, ln) : eq;
                    }
                    legs.emplace_back(match, arm->get_statement());
                    not_any = conjoin(not_any, make_ulnot(match->clone(), fn, ln), fn, ln);
                }
            }
            for(auto &leg : legs) {
                if(!leg.first) {
                    leg.first = not_any;
                }
            }
            for(const auto &leg : legs) {
                bool dead = false;
                const auto &leg_guard = conjoin(guard, leg.first, fn, ln, &dead);
                if(dead) {
                    continue;
                }
                warn_fork_arm_label(leg.second);
                std::vector<Frame> lf = frames;
                push_frame(lf, leg.second);
                if(walk_paths(from, leg_guard, copy_list(action), lf, env, lapped, states, entry)) {
                    return 1;
                }
            }
            if(!has_default) {
                bool dead = false;
                const auto &leg_guard = conjoin(guard, not_any, fn, ln, &dead);
                if(!dead) {
                    std::vector<Frame> lf = frames;
                    if(walk_paths(from, leg_guard, copy_list(action), lf, env, lapped, states,
                                  entry)) {
                        return 1;
                    }
                }
            }
            return 0;
        }

        case AST::NodeType::WhileStatement:
        case AST::NodeType::RepeatStatement:
        case AST::NodeType::ForStatement:
        case AST::NodeType::ForeverStatement:
            // A loop the CFG keeps, registered by the collector: fork at
            // its head (§2, §7.2, §7.3).
            return loop_fork(stmt.get(), true, from, guard, action, frames, env, lapped, states,
                             entry);

        case AST::NodeType::Break:
        case AST::NodeType::Continue: {
            // §8: a jump is just an edge. Unwind to the innermost loop the
            // CFG sees: continue takes its back-edge — the step and the
            // test — break continues past it, skipping both.
            std::size_t depth = frames.size();
            while(depth > 0 && !frames[depth - 1].loop) {
                --depth;
            }
            if(depth == 0) {
                LOG_ERROR_N(stmt) << "break/continue outside a loop the CFG sees: nothing "
                                  << "to jump within";
                return 1;
            }
            const AST::Node *loop = frames[depth - 1].loop;
            std::vector<Frame> unwound(frames.begin(), frames.begin() + (depth - 1));
            if(stmt->is_node_type(AST::NodeType::Continue)) {
                return loop_fork(loop, false, from, guard, action, unwound, env, lapped, states,
                                 entry);
            }
            return walk_paths(from, guard, action, unwound, env, lapped, states, entry);
        }

        case AST::NodeType::EventStatement: {
            const auto &event = AST::cast_to<AST::EventStatement>(stmt);
            const std::size_t idx = m_proc.wait_index.at(event.get());
            // §10.1: the labels in force name the state cut here.
            if(states[idx].stem.empty()) {
                states[idx].stem = labels_of(frames);
            }
            if(record(idx)) {
                return 1;
            }
            // Every path reaches a given wait with the same continuation —
            // its syntactic position fixes it — so its outgoing paths are
            // walked once, on first arrival. The environment and the lap
            // set die with the segment: the next one reads committed
            // registers.
            if(states[idx].walked) {
                return 0;
            }
            states[idx].walked = true;
            std::vector<Frame> cont = frames;
            push_frame(cont, event->get_statement());
            return walk_paths(idx, nullptr, std::make_shared<AST::Node::List>(), cont, Env{}, {},
                              states, entry);
        }

        default:
            LOG_ERROR_N(stmt) << "construct not handled by the FSM lowering: "
                              << stmt->get_node_type();
            return 1;
        }
    }

    // The process ends: a one-shot parks in the hold state (§2).
    return record(states.size());
}

AST::Var::Ptr ImplicitFsmElaboration::find_temp_decl(const std::vector<Frame> &frames,
                                                     const std::string &name)
{
    for(auto it = frames.rbegin(); it != frames.rend(); ++it) {
        for(const auto &decl : it->decls) {
            if(decl->get_name() == name) {
                return decl;
            }
        }
    }
    return nullptr;
}

int ImplicitFsmElaboration::process_blocking(const AST::Var::Ptr &decl, const std::string &target,
                                             const AST::Node::Ptr &rhs, Env &env,
                                             const std::string &fn, int ln)
{
    const auto &value = clone_subst(rhs, env);
    if(check_temp_reads(value, env)) {
        return 1;
    }
    // §6.1: a constant folds inline, truncated to the declared width at
    // substitution time — the one value that never earns a wire. Signed
    // temporaries and widths past 32 keep the wire, whose typed
    // declaration says the same thing without arithmetic here.
    unsigned int width = 0;
    mpz_class folded;
    const bool has_signing =
        decl->get_type() && decl->get_type()->get_signing() != AST::DataType::SigningEnum::NONE;
    const bool atom_signed =
        decl->get_type() && (decl->get_type()->is_node_type(AST::NodeType::IntType) ||
                             decl->get_type()->is_node_type(AST::NodeType::IntegerType) ||
                             decl->get_type()->is_node_type(AST::NodeType::ByteType) ||
                             decl->get_type()->is_node_type(AST::NodeType::ShortintType) ||
                             decl->get_type()->is_node_type(AST::NodeType::LongintType));
    const bool is_signed = (decl->get_type() && decl->get_type()->get_signing() ==
                                                    AST::DataType::SigningEnum::SIGNED) ||
                           (atom_signed && !has_signing);
    if(!is_signed && !declared_width(decl, width) && width >= 1 && width <= 32 &&
       ExpressionEvaluation().evaluate_node(value, folded)) {
        const mpz_class modulus = mpz_class(1) << width;
        const mpz_class masked = ((folded % modulus) + modulus) % modulus;
        env[target] = AST::to_node(
            make_const(masked.convert_to<unsigned int>(), static_cast<int>(width), fn, ln));
        return 0;
    }
    env[target] = AST::to_node(make_id(materialize_temp(decl, target, value), fn, ln));
    return 0;
}

std::string ImplicitFsmElaboration::materialize_temp(const AST::Var::Ptr &decl,
                                                     const std::string &temp,
                                                     const AST::Node::Ptr &value)
{
    // §6.1: identical expressions share one wire — naming, not binding —
    // but only under the SAME declared type: the type is the truncation.
    for(const auto &wire : m_proc.wires) {
        const bool same_type = (!wire.temp->get_type() && !decl->get_type()) ||
                               (wire.temp->get_type() && decl->get_type() &&
                                wire.temp->get_type()->is_equal(decl->get_type(), false));
        if(same_type && wire.value->is_equal(value, false)) {
            return wire.name;
        }
    }
    const std::string base = m_proc.prefix + "_t_" + temp;
    std::string name = base;
    std::size_t ordinal = 0;
    for(bool taken = true; taken;) {
        taken = false;
        for(const auto &wire : m_proc.wires) {
            if(wire.name == name) {
                name = base + "_" + std::to_string(++ordinal);
                taken = true;
                break;
            }
        }
    }
    m_proc.wires.push_back(MaterializedWire{name, value, decl});
    return name;
}

int ImplicitFsmElaboration::check_temp_reads(const AST::Node::Ptr &node, const Env &env)
{
    if(m_proc.temps.empty() && env.empty()) {
        return 0;
    }
    std::set<std::string> reads;
    switch(node->get_node_type()) {
    case AST::NodeType::Block:
    case AST::NodeType::NonblockingSubstitution:
    case AST::NodeType::IfStatement:
    case AST::NodeType::CaseStatement:
    case AST::NodeType::CasexStatement:
    case AST::NodeType::CasezStatement:
        // Statement position: assignment targets are writes, not reads.
        collect_reads(node, reads);
        break;
    default:
        collect_identifier_names(node, reads);
        break;
    }
    for(const auto &read : reads) {
        const auto &found = env.find(read);
        if(found != env.end() && !found->second) {
            LOG_ERROR_N(node) << "'" << read << "' is read in the cycle of a conditional "
                              << "task call that assigns it: the copied value depends on the "
                              << "branch — read it after the next wait, or hoist the call out "
                              << "of the branch";
            return 1;
        }
        if(m_proc.temps.count(read) && !env.count(read)) {
            LOG_ERROR_N(node) << "temporary '" << read << "' is read before it is "
                              << "assigned in this segment — or outside the scope that "
                              << "declares it";
            return 1;
        }
    }
    return 0;
}

void ImplicitFsmElaboration::push_induced(const AST::Node::ListPtr &action,
                                          const std::string &target, const AST::Node::Ptr &rhs,
                                          const std::string &fn, int ln)
{
    for(auto it = action->begin(); it != action->end();) {
        const bool induced = m_proc.induced.count(it->get()) &&
                             (*it)->is_node_type(AST::NodeType::NonblockingSubstitution) &&
                             nba_target(AST::cast_to<AST::NonblockingSubstitution>(*it)) == target;
        it = induced ? action->erase(it) : std::next(it);
    }
    const auto &commit = AST::to_node(make_nba(target, rhs, fn, ln));
    m_proc.induced.insert(commit.get());
    action->push_back(commit);
}

int ImplicitFsmElaboration::loop_fork(const AST::Node *loop, bool entering, std::size_t from,
                                      const AST::Node::Ptr &guard, const AST::Node::ListPtr &action,
                                      const std::vector<Frame> &frames, const Env &env,
                                      const std::set<const AST::Node *> &lapped,
                                      std::vector<State> &states, std::vector<Transition> &entry)
{
    const auto &info = m_proc.loops.at(loop);
    const auto &anchor = info.cond ? info.cond : info.body;
    const std::string &fn = anchor->get_filename();
    const int ln = anchor->get_line();

    // Entering the body: a lap that reached this head again without
    // crossing a cut point re-enters in zero time (IEEE §9.2.2.1, §9).
    const auto enter_body = [&](const AST::Node::Ptr &leg_guard, AST::Node::ListPtr leg_action,
                                Env leg_env) -> int {
        if(lapped.count(loop)) {
            LOG_ERROR_N(anchor) << "a path through this loop's body reaches the loop head again "
                                << "without crossing a cut point: a zero-delay lap "
                                << "(IEEE 1800-2017 §9.2.2.1). If a rolled "
                                << "repeat with a possibly-zero count sits on that path, "
                                << "its skip is the lap: a cut point after it breaks it";
            return 1;
        }
        auto leg_lapped = lapped;
        leg_lapped.insert(loop);
        auto leg_frames = frames;
        push_frame(leg_frames, info.body, loop);
        return walk_paths(from, leg_guard, leg_action, leg_frames, leg_env, leg_lapped, states,
                          entry);
    };
    const auto skip_past = [&](const AST::Node::Ptr &leg_guard, AST::Node::ListPtr leg_action,
                               Env leg_env) -> int {
        return walk_paths(from, leg_guard, leg_action, frames, leg_env, lapped, states, entry);
    };

    switch(info.kind) {
    case LoopInfo::Kind::WHILE: {
        // A constant test folds at the head: while (1) is the perpetual
        // form spelled differently (§2) — no dead exit leg, no hold state
        // it would otherwise force — and while (0) never runs.
        mpz_class value;
        if(ExpressionEvaluation().evaluate_node(info.cond, value)) {
            if(value == 0) {
                return skip_past(guard, copy_list(action), env);
            }
            return enter_body(guard, action, env);
        }
        // §7.3: the test re-evaluates at the head, entry and back-edge
        // alike, over the segment's values.
        {
            bool dead = false;
            const auto &cond_subst = clone_subst(info.cond, env);
            if(check_temp_reads(cond_subst, env)) {
                return 1;
            }
            const auto &leg_guard = conjoin(guard, cond_subst, fn, ln, &dead);
            if(!dead && enter_body(leg_guard, copy_list(action), env)) {
                return 1;
            }
        }
        bool dead = false;
        const auto &leg_guard =
            conjoin(guard, make_ulnot(clone_subst(info.cond, env), fn, ln), fn, ln, &dead);
        return dead ? 0 : skip_past(leg_guard, copy_list(action), env);
    }

    case LoopInfo::Kind::FOREVER:
        // §2: the §7.3 back-edge with no exit test — the only way past it
        // is a break (§8).
        return enter_body(guard, action, env);

    case LoopInfo::Kind::REPEAT: {
        // A repeat that induces no counter (count 0 or 1) has no width
        // slot at its depth; its legs never touch the countdown.
        const std::string depth_cnt = cnt_name(info.depth);
        const unsigned int depth_width =
            info.depth < m_proc.cnt_widths.size() ? m_proc.cnt_widths[info.depth] : 0;
        const auto cnt_id = [&]() { return AST::to_node(make_id(depth_cnt, fn, ln)); };
        const auto cnt_zero = [&]() {
            return AST::to_node(make_const(0, static_cast<int>(depth_width), fn, ln));
        };
        if(entering) {
            if(info.count_known) {
                if(info.count_value == 0) {
                    return skip_past(guard, action, env);
                }
                if(info.count_value == 1) {
                    // §7.2: a single pass needs no countdown, but the body
                    // still owns its jumps (§8): it enters as a loop whose
                    // back-edge exits unconditionally.
                    return enter_body(guard, action, env);
                }
                auto leg_action = copy_list(action);
                push_induced(
                    leg_action, depth_cnt,
                    AST::to_node(make_const(static_cast<unsigned int>(info.count_value - 1),
                                            static_cast<int>(depth_width), fn, ln)),
                    fn, ln);
                return enter_body(guard, leg_action, env);
            }
            // §12.7.2: the count is evaluated once, on entry — captured
            // into the countdown — and a zero count skips the state
            // through the entry guard.
            {
                bool dead = false;
                const auto &leg_guard =
                    conjoin(guard, make_noteq(clone_subst(info.cond, env), cnt_zero(), fn, ln), fn,
                            ln, &dead);
                if(!dead) {
                    auto leg_action = copy_list(action);
                    push_induced(leg_action, depth_cnt,
                                 make_minus(clone_subst(info.cond, env),
                                            AST::to_node(make_const(
                                                1, static_cast<int>(depth_width), fn, ln)),
                                            fn, ln),
                                 fn, ln);
                    if(enter_body(leg_guard, leg_action, env)) {
                        return 1;
                    }
                }
            }
            bool dead = false;
            const auto &leg_guard = conjoin(
                guard, make_eq(clone_subst(info.cond, env), cnt_zero(), fn, ln), fn, ln, &dead);
            return dead ? 0 : skip_past(leg_guard, copy_list(action), env);
        }
        // Back-edge of the single pass: exit unconditionally — which is
        // also where a continue inside it lands (§12.7.2: no next lap).
        if(info.count_known && info.count_value == 1) {
            return skip_past(guard, copy_list(action), env);
        }
        // Back-edge: the countdown decides, and decrements on the lap.
        {
            bool dead = false;
            const auto &leg_guard =
                conjoin(guard, make_noteq(cnt_id(), cnt_zero(), fn, ln), fn, ln, &dead);
            if(!dead) {
                auto leg_action = copy_list(action);
                push_induced(
                    leg_action, depth_cnt,
                    make_minus(cnt_id(),
                               AST::to_node(make_const(1, static_cast<int>(depth_width), fn, ln)),
                               fn, ln),
                    fn, ln);
                if(enter_body(leg_guard, leg_action, env)) {
                    return 1;
                }
            }
        }
        bool dead = false;
        const auto &leg_guard =
            conjoin(guard, make_eq(cnt_id(), cnt_zero(), fn, ln), fn, ln, &dead);
        return dead ? 0 : skip_past(leg_guard, copy_list(action), env);
    }

    case LoopInfo::Kind::FOR: {
        // §7.2: the init or the step commits once, and its value is
        // substituted forward within its own segment (§6.1) — which is when
        // the source evaluates the test.
        const auto &value = clone_subst(entering ? info.init_rhs : info.step_rhs, env);
        Env leg_env = env;
        leg_env[info.index] = value;
        auto base = copy_list(action);
        push_induced(base, info.index, value->clone(), fn, ln);
        {
            bool dead = false;
            const auto &leg_guard = conjoin(guard, clone_subst(info.cond, leg_env), fn, ln, &dead);
            if(!dead && enter_body(leg_guard, copy_list(base), leg_env)) {
                return 1;
            }
        }
        bool dead = false;
        const auto &leg_guard =
            conjoin(guard, make_ulnot(clone_subst(info.cond, leg_env), fn, ln), fn, ln, &dead);
        return dead ? 0 : skip_past(leg_guard, base, leg_env);
    }
    }
    return 1;
}

/// The induced commits travelled as pragma markers, which any cloning
/// copies along; with the process's text final, each marker unwraps to
/// its nonblocking assignment and the walk's induced index takes the
/// pointer — stable from here, nothing clones again.
void ImplicitFsmElaboration::adopt_markers(const AST::Node::Ptr &node)
{
    if(!node) {
        return;
    }
    const auto &children = node->get_children();
    if(!children) {
        return;
    }
    for(const auto &child : *children) {
        const int kind = induced_marker_kind(child);
        if(kind == 0) {
            adopt_markers(child);
            continue;
        }
        const auto &nba = induced_marker_nba(child);
        if(!nba) {
            continue;
        }
        node->replace(child, AST::to_node(nba));
        if(kind == 1) {
            m_inline.captures.insert(nba.get());
        } else {
            m_inline.copyouts.insert(nba.get());
        }
    }
}

bool ImplicitFsmElaboration::contains_induced(const AST::Node::Ptr &node) const
{
    if(!node) {
        return false;
    }
    if(m_inline.captures.count(node.get()) || m_inline.copyouts.count(node.get())) {
        return true;
    }
    const auto &children = node->get_children();
    if(!children) {
        return false;
    }
    for(const auto &child : *children) {
        if(contains_induced(child)) {
            return true;
        }
    }
    return false;
}

/// A cut-point-free subtree holding induced commits, emitted into an
/// action: reads substitute against a branch-local environment that the
/// captures and copy-outs update in program order — §13.3's immediate
/// visibility inside the branch — while each nested arm diverges on a
/// copy. The caller marks the committed targets branch-dependent in the
/// segment's own environment.
AST::Node::Ptr ImplicitFsmElaboration::emit_verbatim(const AST::Node::Ptr &stmt, Env &env,
                                                     std::set<std::string> &committed)
{
    const std::string &fn = stmt->get_filename();
    const int ln = stmt->get_line();

    if(stmt->is_node_type(AST::NodeType::NonblockingSubstitution) &&
       (m_inline.captures.count(stmt.get()) || m_inline.copyouts.count(stmt.get()))) {
        const auto &nba = AST::cast_to<AST::NonblockingSubstitution>(stmt);
        const auto &value = clone_subst(AST::to_node(nba->get_right()->get_var()), env);
        if(check_temp_reads(value, env)) {
            return nullptr;
        }
        env[nba_target(nba)] = value;
        committed.insert(nba_target(nba));
        return AST::to_node(make_nba(nba_target(nba), value, fn, ln));
    }

    if(stmt->is_node_type(AST::NodeType::Block)) {
        const auto &blk = AST::cast_to<AST::Block>(stmt);
        auto out = std::make_shared<AST::Node::List>();
        if(blk->get_statements()) {
            for(const auto &child : *blk->get_statements()) {
                if(!child) {
                    continue;
                }
                if(child->is_node_type(AST::NodeType::Var)) {
                    out->push_back(child->clone());
                    continue;
                }
                const auto &emitted = emit_verbatim(child, env, committed);
                if(!emitted) {
                    return nullptr;
                }
                out->push_back(emitted);
            }
        }
        return AST::to_node(std::make_shared<AST::Block>(out, blk->get_scope(), fn, ln));
    }

    if(stmt->is_node_type(AST::NodeType::SingleStatement)) {
        const auto &single = AST::cast_to<AST::SingleStatement>(stmt);
        if(single->get_statement() && contains_induced(single->get_statement())) {
            const auto &inner = emit_verbatim(single->get_statement(), env, committed);
            if(!inner) {
                return nullptr;
            }
            const auto &out = AST::cast_to<AST::SingleStatement>(stmt->clone());
            out->set_statement(inner);
            return AST::to_node(out);
        }
    }

    if(stmt->is_node_type(AST::NodeType::IfStatement) && contains_induced(stmt)) {
        const auto &ifs = AST::cast_to<AST::IfStatement>(stmt);
        const auto &cond = clone_subst(ifs->get_cond(), env);
        if(check_temp_reads(cond, env)) {
            return nullptr;
        }
        AST::Node::Ptr t;
        AST::Node::Ptr e;
        std::set<std::string> arm_committed;
        if(ifs->get_true_statement()) {
            Env arm_env = env;
            t = emit_verbatim(ifs->get_true_statement(), arm_env, arm_committed);
            if(!t) {
                return nullptr;
            }
        }
        if(ifs->get_false_statement()) {
            Env arm_env = env;
            e = emit_verbatim(ifs->get_false_statement(), arm_env, arm_committed);
            if(!e) {
                return nullptr;
            }
        }
        for(const auto &target : arm_committed) {
            env[target] = nullptr; // arm-dependent past this if
            committed.insert(target);
        }
        auto out = std::make_shared<AST::IfStatement>(fn, ln);
        out->set_cond(cond);
        out->set_true_statement(t);
        out->set_false_statement(e);
        return AST::to_node(out);
    }

    if(contains_induced(stmt)) {
        LOG_ERROR_N(stmt) << "a task call under this construct, inside a branch no cut point "
                          << "spans, is not lowered — hoist the call out of the branch, or put "
                          << "a wait inside it";
        return nullptr;
    }

    const auto &placed = clone_subst(stmt, env);
    if(check_temp_reads(placed, env)) {
        return nullptr;
    }
    return placed;
}

int ImplicitFsmElaboration::compile_process(const AST::Module::Ptr &module,
                                            const AST::Node::Ptr &parent,
                                            const AST::Pragmalist::Ptr &pragmalist,
                                            const AST::Initial::Ptr &initial,
                                            const std::string &prefix)
{
    // One state per wait, in source order (§4, §10.1); each wait is checked
    // for shape and clock uniformity as it is collected. Reconstruction is
    // the reset: a member added to the per-process state can never be
    // missed by a clear list.
    m_proc = ProcessState{};
    m_inline = InlineState{};
    m_proc.cnt_name = prefix + "_cnt";
    m_proc.prefix = prefix;
    m_proc.module = module;
    m_proc.pragmalist = pragmalist;

    // §3: veriparse_encoding picks the state constants' shape.
    const auto &encoding = get_pragma(pragmalist, "veriparse_encoding");
    if(encoding) {
        const auto &expr = encoding->get_expression();
        std::string wanted;
        if(expr && expr->is_node_type(AST::NodeType::StringConst)) {
            wanted = AST::cast_to<AST::StringConst>(expr)->get_value();
        }
        if(wanted == "binary") {
            m_proc.encoding = Encoding::BINARY;
        } else if(wanted == "one_hot") {
            m_proc.encoding = Encoding::ONE_HOT;
        } else if(wanted == "gray") {
            m_proc.encoding = Encoding::GRAY;
        } else if(wanted == "output") {
            m_proc.encoding = Encoding::OUTPUT;
        } else {
            LOG_ERROR_N(pragmalist) << "veriparse_encoding is \"binary\", \"one_hot\", "
                                    << "\"gray\" or \"output\"";
            return 1;
        }
    }
    // The inliner ran per module; its induced commits arrived as pragma
    // markers, adopted here — unwrapped, and indexed by pointer for the
    // walk, stable because nothing clones after this point.
    adopt_markers(AST::to_node(initial));

    AST::Sens::Ptr clock;
    std::vector<AST::EventStatement::Ptr> waits;
    bool has_wait = false;
    if(collect_body(initial->get_statement(), waits, clock, has_wait, TempScope{})) {
        return 1;
    }

    if(!clock) {
        LOG_ERROR_N(initial) << "(* veriparse_fsm *) on an initial with no wait: "
                             << "there is nothing to compile";
        return 1;
    }

    if(check_called_functions(module, initial)) {
        return 1;
    }

    // §6: a temporary named after a module-level declaration — a register,
    // a port, a rolled for's index — is legal SystemVerilog shadowing, but
    // substitution and the by-name checks bind by name. Rejected, with the
    // one-word fix.
    for(const auto &elt : m_proc.temps) {
        if(find_declaration(module, elt.first)) {
            LOG_ERROR_N(elt.second)
                << "temporary '" << elt.first << "' shadows a module-level declaration: "
                << "substitution binds by name — rename it";
            return 1;
        }
    }

    // §9 / IEEE §9.2.2.4: no other process or continuous assign may write a
    // register this machine drives — the source is merely a race, but the
    // emitted always_ff would not conform, the stronger reason to refuse.
    {
        std::set<std::string> mine;
        collect_driven(initial->get_statement(), mine);
        for(const auto &elt : m_proc.loops) {
            if(elt.second.kind == LoopInfo::Kind::FOR) {
                mine.insert(elt.second.index);
            }
        }
        // §6 temporaries dissolve into values: they are not registers of
        // this process, and a same-named variable elsewhere is unrelated.
        for(const auto &elt : m_proc.temps) {
            mine.erase(elt.first);
        }
        std::set<std::string> others;
        collect_foreign_drivers(module, pragmalist, m_modules, others);
        for(const auto &name : mine) {
            if(others.count(name)) {
                LOG_ERROR_N(initial)
                    << "register '" << name << "' is also written by another process "
                    << "or a continuous assign: the emitted always_ff would not "
                    << "conform (IEEE 1800-2017 §9.2.2.4)";
                return 1;
            }
        }
    }

    // Size the shared countdowns (§15: one per repeat-nesting depth)
    // over every rolled
    // repeat: $clog2(N) for a folded count, the count signal's declared
    // width otherwise — the capture `cnt <= expr - 1` must hold any value
    // the signal can carry. Rolled for indices are the author's registers:
    // they must exist at module level, with their declared type (§7.2).
    for(const auto &elt : m_proc.loops) {
        const auto &info = elt.second;
        if(info.kind == LoopInfo::Kind::FOR) {
            bool is_input = false;
            const auto &decl = find_declaration(module, info.index, &is_input);
            if(!decl) {
                LOG_ERROR_N(info.cond)
                    << "rolled for index '" << info.index << "' is not a module-level "
                    << "declaration: the induced register takes the index's declared "
                    << "type";
                return 1;
            }
            // The machine drives the index from its always_ff: an input
            // port or a net cannot take the commits.
            if(is_input || !std::dynamic_pointer_cast<AST::Var>(decl)) {
                LOG_ERROR_N(info.cond)
                    << "rolled for index '" << info.index << "' is not a variable the "
                    << "machine can drive: an input port or a net cannot take the "
                    << "induced register's commits";
                return 1;
            }
            continue;
        }
        if(info.kind != LoopInfo::Kind::REPEAT) {
            continue;
        }
        const auto widen = [this](unsigned int depth, unsigned int width) {
            if(m_proc.cnt_widths.size() <= depth) {
                m_proc.cnt_widths.resize(depth + 1, 0);
            }
            m_proc.cnt_widths[depth] = std::max(m_proc.cnt_widths[depth], width);
        };
        if(info.count_known) {
            if(info.count_value >= 2) {
                widen(info.depth, clog2(static_cast<unsigned int>(info.count_value)));
            }
            continue;
        }
        if(!info.cond->is_node_type(AST::NodeType::Identifier)) {
            LOG_ERROR_N(info.cond) << "non-constant repeat count must be a plain signal, so the "
                                   << "countdown can take its declared width — bind the expression "
                                   << "to a named signal first";
            return 1;
        }
        const auto &count_name = AST::cast_to<AST::Identifier>(info.cond)->get_name();
        const auto &decl = find_declaration(module, count_name);
        unsigned int width = 0;
        if(!decl || declared_width(decl, width)) {
            LOG_ERROR_N(info.cond) << "cannot size the countdown for repeat count '" << count_name
                                   << "': its declaration or packed range is not resolvable";
            return 1;
        }
        widen(info.depth, width);
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

    // Enumerate the path cover (§C.4): each state's outgoing transitions,
    // plus the entry paths from the top of the process to its first wait.
    std::vector<State> states;
    for(const auto &wait : waits) {
        states.push_back(State{wait, {}, false});
    }
    std::vector<Transition> entry;
    std::vector<Frame> frames;
    push_frame(frames, initial->get_statement());
    if(walk_paths(k_entry, nullptr, std::make_shared<AST::Node::List>(), frames, Env{}, {}, states,
                  entry)) {
        return 1;
    }

    // The statements before the first wait are the reset branch (§5.1): one
    // unconditional path. A branch there whose arms hold cut points would
    // make the state out of reset input-dependent.
    if(entry.size() != 1 || entry.front().guard) {
        // A fork whose arms all reach the same first wait carries no cut
        // point: it landed here because a decoded output takes a
        // conditional value in the init segment (§6.2 forces such a
        // branch onto the path cover).
        bool same_next = true;
        for(const auto &leg : entry) {
            same_next &= leg.next == entry.front().next;
        }
        if(same_next && !m_proc.decoded.empty()) {
            LOG_ERROR_N(initial)
                << "a decoded output takes a conditional value in the init segment: the "
                << "reset value is unconditional — assign it once before the branch, or "
                << "move the branch after the first wait";
        } else {
            LOG_ERROR_N(initial) << "a cut point inside a branch before the first wait: "
                                 << "the reset branch cannot fork";
        }
        return 1;
    }
    const auto &init_stmts = entry.front().action;
    const std::size_t entry_next = entry.front().next;

    if(check_paths(init_stmts, states, entry_next, enable)) {
        return 1;
    }

    if(build_decode(states, entry, entry_next)) {
        return 1;
    }

    const auto &emitted =
        emit(module, clock, enable, reset_name, active_low, init_stmts, states, entry_next, prefix);
    if(!emitted) {
        return 1;
    }

    AST::Node::Ptr pragmalist_node = AST::to_node(pragmalist);
    AST::Node::Ptr parent_node = parent;
    pickup_statements(parent_node, pragmalist_node, emitted);
    return 0;
}

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

    // §6.2: a rolled counting loop's lap is a path with no user statement,
    // so it can re-assert nothing — totality cannot hold across it. §7.3's
    // while idiom is the spelling that re-asserts per lap.
    for(const auto &elt : m_proc.loops) {
        const auto &info = elt.second;
        const bool counting =
            info.kind == LoopInfo::Kind::FOR ||
            (info.kind == LoopInfo::Kind::REPEAT && (!info.count_known || info.count_value >= 2));
        if(counting) {
            LOG_ERROR_N(info.body)
                << "a rolled loop's lap cannot re-assert a decoded output: every path "
                << "between two cut points must assign it — spell the loop as a while "
                << "re-asserting the output per lap, or keep the output a register";
            return 1;
        }
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
        for(std::size_t depth = 0; depth < m_proc.cnt_widths.size(); ++depth) {
            if(m_proc.cnt_widths[depth] > 0) {
                unique.insert(cnt_name(static_cast<unsigned int>(depth)));
                ++expected;
            }
        }
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

    // logic [w-1:0] <prefix>_cnt, _cnt2, ...; — one shared countdown per
    // repeat-nesting depth that induced one (§7.2, §15).
    for(std::size_t depth = 0; depth < m_proc.cnt_widths.size(); ++depth) {
        if(m_proc.cnt_widths[depth] == 0) {
            continue;
        }
        const auto &name = cnt_name(static_cast<unsigned int>(depth));
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(name, declared)) {
            LOG_ERROR_N(module) << "generated declaration '" << name
                                << "' collides with an existing one";
            return nullptr;
        }
        auto cnt_type = std::make_shared<AST::LogicType>(fn, ln);
        if(m_proc.cnt_widths[depth] > 1) {
            cnt_type->set_packed_dims(make_packed_range(m_proc.cnt_widths[depth] - 1, fn, ln));
        }
        auto cnt = std::make_shared<AST::Var>(fn, ln);
        cnt->set_name(name);
        cnt->set_type(cnt_type);
        result->push_back(AST::to_node(cnt));
    }

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
