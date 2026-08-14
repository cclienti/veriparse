// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/fsm_loop_lowering.hpp>
#include "implicit_fsm_detail.hpp"

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

using namespace FsmDetail;

namespace
{

/// Rewrite every `continue` bound to the loop being lowered — not one of
/// a nested loop — through `rewrite`: a rolled for's continue must still
/// step its index, a run-once repeat's continue is its exit.
void rewrite_continues(const AST::Node::Ptr &node,
                       const std::function<AST::Node::Ptr(const AST::Node::Ptr &)> &rewrite)
{
    if(!node) {
        return;
    }
    const auto &children = node->get_children();
    if(!children) {
        return;
    }
    for(const auto &child : *children) {
        if(!child) {
            continue;
        }
        switch(child->get_node_type()) {
        case AST::NodeType::WhileStatement:
        case AST::NodeType::ForStatement:
        case AST::NodeType::RepeatStatement:
        case AST::NodeType::ForeverStatement:
            continue; // an inner loop owns its continues
        case AST::NodeType::Continue:
            node->replace(child, rewrite(child));
            continue;
        case AST::NodeType::SingleStatement: {
            const auto &single = AST::cast_to<AST::SingleStatement>(child);
            if(single->get_statement() &&
               single->get_statement()->is_node_type(AST::NodeType::Continue)) {
                node->replace(child, rewrite(single->get_statement()));
                continue;
            }
            rewrite_continues(child, rewrite);
            continue;
        }
        default:
            rewrite_continues(child, rewrite);
        }
    }
}

AST::Node::Ptr block_of(const AST::Node::ListPtr &stmts, const std::string &fn, int ln)
{
    return AST::to_node(std::make_shared<AST::Block>(stmts, "", fn, ln));
}

} // namespace

int FsmLoopLowering::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }
    if(!node->is_node_type(AST::NodeType::Module)) {
        return recurse_in_childs(node);
    }
    return lower_module(AST::cast_to<AST::Module>(node));
}

int FsmLoopLowering::lower_module(const AST::Module::Ptr &module)
{
    m_module = module;
    m_declared.clear();

    std::vector<std::pair<AST::Pragmalist::Ptr, AST::Initial::Ptr>> initials;
    for(const auto &pair : collect_marked(module)) {
        if(pair.second->is_node_type(AST::NodeType::Initial)) {
            initials.emplace_back(pair.first, AST::cast_to<AST::Initial>(pair.second));
        }
    }
    if(initials.empty()) {
        return 0;
    }
    if(Analysis::UniqueDeclaration::analyze(AST::to_node(module), m_declared)) {
        LOG_ERROR_N(module) << "failed to analyze declarations";
        return 1;
    }

    for(std::size_t i = 0; i < initials.size(); ++i) {
        m_pragmalist = initials[i].first;
        m_prefix = marked_prefix(initials[i].first, i, initials.size());
        if(lower_process(initials[i].second)) {
            return 1;
        }
    }
    return 0;
}

std::string FsmLoopLowering::cnt_name(unsigned int depth) const
{
    const std::string base = m_prefix + "_cnt";
    return depth == 0 ? base : base + std::to_string(depth + 1);
}

int FsmLoopLowering::lower_process(const AST::Initial::Ptr &initial)
{
    m_depth = 0;
    m_cnt_widths.clear();

    AST::Node::Ptr body = initial->get_statement();
    if(lower_slot(body)) {
        return 1;
    }
    initial->set_statement(body);

    // logic [w-1:0] <prefix>_cnt, _cnt2, ...; — one shared countdown per
    // repeat-nesting depth that induced one (§7.2, §15), declared before
    // the marked process like every other hoist.
    const auto &items = m_module->get_items();
    for(std::size_t depth = 0; depth < m_cnt_widths.size(); ++depth) {
        if(m_cnt_widths[depth] == 0) {
            continue;
        }
        const std::string name = cnt_name(static_cast<unsigned int>(depth));
        if(Analysis::UniqueDeclaration::identifier_declaration_exists(name, m_declared)) {
            LOG_ERROR_N(m_module) << "generated declaration '" << name
                                  << "' collides with an existing one";
            return 1;
        }
        m_declared.insert(name);
        auto cnt_type = std::make_shared<AST::LogicType>(m_module->get_filename(), 0);
        if(m_cnt_widths[depth] > 1) {
            cnt_type->set_packed_dims(
                make_packed_range(m_cnt_widths[depth] - 1, m_module->get_filename(), 0));
        }
        auto cnt = std::make_shared<AST::Var>(m_module->get_filename(), 0);
        cnt->set_name(name);
        cnt->set_type(cnt_type);
        if(items) {
            bool placed = false;
            for(auto it = items->begin(); it != items->end(); ++it) {
                if(it->get() == static_cast<AST::Node *>(m_pragmalist.get())) {
                    items->insert(it, AST::to_node(cnt));
                    placed = true;
                    break;
                }
            }
            if(!placed) {
                items->push_back(AST::to_node(cnt));
            }
        }
    }
    return 0;
}

/// A single-statement slot (a branch, a loop body): lowered like a list;
/// a loop lowering to several statements takes an unnamed block.
int FsmLoopLowering::lower_slot(AST::Node::Ptr &slot)
{
    if(!slot) {
        return 0;
    }
    auto list = std::make_shared<AST::Node::List>();
    list->push_back(slot);
    if(lower_list(list)) {
        return 1;
    }
    if(list->size() == 1) {
        slot = list->front();
        return 0;
    }
    slot = block_of(list, slot ? slot->get_filename() : "", slot ? slot->get_line() : 0);
    return 0;
}

int FsmLoopLowering::lower_list(const AST::Node::ListPtr &stmts)
{
    if(!stmts) {
        return 0;
    }
    for(auto it = stmts->begin(); it != stmts->end();) {
        AST::Node::Ptr s = *it;
        if(!s || induced_marker_kind(s)) {
            ++it;
            continue;
        }

        bool kept_rolled = false;
        AST::Node::Ptr loop = s;
        if(s->is_node_type(AST::NodeType::Pragmalist)) {
            const auto &pragmalist = AST::cast_to<AST::Pragmalist>(s);
            if(has_pragma(pragmalist, "veriparse_no_unroll") && pragmalist->get_statements() &&
               pragmalist->get_statements()->size() == 1) {
                kept_rolled = true;
                loop = pragmalist->get_statements()->front();
            } else {
                if(lower_list(pragmalist->get_statements())) {
                    return 1;
                }
                ++it;
                continue;
            }
        }

        if(loop->is_node_type(AST::NodeType::RepeatStatement)) {
            auto lowered = std::make_shared<AST::Node::List>();
            if(lower_repeat(AST::cast_to<AST::RepeatStatement>(loop), kept_rolled, lowered)) {
                return 1;
            }
            it = stmts->erase(it);
            stmts->insert(it, lowered->begin(), lowered->end());
            continue;
        }
        if(loop->is_node_type(AST::NodeType::ForStatement)) {
            auto lowered = std::make_shared<AST::Node::List>();
            if(lower_for(AST::cast_to<AST::ForStatement>(loop), kept_rolled, lowered)) {
                return 1;
            }
            it = stmts->erase(it);
            stmts->insert(it, lowered->begin(), lowered->end());
            continue;
        }

        // Anything else: the loops hide in its statement slots.
        switch(s->get_node_type()) {
        case AST::NodeType::Block:
            if(lower_list(AST::cast_to<AST::Block>(s)->get_statements())) {
                return 1;
            }
            break;
        case AST::NodeType::IfStatement: {
            const auto &ifs = AST::cast_to<AST::IfStatement>(s);
            AST::Node::Ptr t = ifs->get_true_statement();
            AST::Node::Ptr e = ifs->get_false_statement();
            if(lower_slot(t) || lower_slot(e)) {
                return 1;
            }
            ifs->set_true_statement(t);
            ifs->set_false_statement(e);
            break;
        }
        case AST::NodeType::CaseStatement:
        case AST::NodeType::CasexStatement:
        case AST::NodeType::CasezStatement: {
            const auto &caselist = AST::cast_to<AST::CaseStatement>(s)->get_caselist();
            if(caselist) {
                for(const auto &arm : *caselist) {
                    AST::Node::Ptr stmt = arm->get_statement();
                    if(lower_slot(stmt)) {
                        return 1;
                    }
                    arm->set_statement(stmt);
                }
            }
            break;
        }
        case AST::NodeType::WhileStatement: {
            const auto &loop_stmt = AST::cast_to<AST::WhileStatement>(s);
            AST::Node::Ptr body = loop_stmt->get_statement();
            if(lower_slot(body)) {
                return 1;
            }
            loop_stmt->set_statement(body);
            break;
        }
        case AST::NodeType::ForeverStatement: {
            const auto &loop_stmt = AST::cast_to<AST::ForeverStatement>(s);
            AST::Node::Ptr body = loop_stmt->get_statement();
            if(lower_slot(body)) {
                return 1;
            }
            loop_stmt->set_statement(body);
            break;
        }
        case AST::NodeType::SingleStatement: {
            const auto &single = AST::cast_to<AST::SingleStatement>(s);
            AST::Node::Ptr inner = single->get_statement();
            if(lower_slot(inner)) {
                return 1;
            }
            single->set_statement(inner);
            break;
        }
        default:
            break;
        }
        ++it;
    }
    return 0;
}

/// The refusals every rolled bounded loop shares: a loop without a cut
/// point has no state to save, and rolled is opt-in (§7.2, §8).
int FsmLoopLowering::check_rolled(const AST::Node::Ptr &loop, bool kept_rolled) const
{
    const AST::Node::Ptr body = loop->is_node_type(AST::NodeType::RepeatStatement)
                                    ? AST::cast_to<AST::RepeatStatement>(loop)->get_statement()
                                    : AST::cast_to<AST::ForStatement>(loop)->get_statement();
    if(!contains_event_statement(body)) {
        if(kept_rolled) {
            LOG_ERROR_N(loop) << "(* veriparse_no_unroll *) on a loop without a cut point: "
                              << "the loop runs in zero time and there is no state to save — "
                              << "drop the hint and let it unroll";
        } else {
            LOG_ERROR_N(loop) << "loop with no cut point survived to the FSM lowering: no "
                              << "static exit, or the unroller refused it — a zero-delay "
                              << "loop has no hardware meaning (IEEE 1800-2017 §9.2.2.1)";
        }
        return 1;
    }
    if(!kept_rolled) {
        LOG_ERROR_N(loop) << "bounded loop with a cut point was not unrolled upstream "
                          << "(non-constant bound, or a jump shape the unroller refuses): "
                          << "mark it (* veriparse_no_unroll *) to compile it rolled, or "
                          << "make the bound constant";
        return 1;
    }
    return 0;
}

int FsmLoopLowering::lower_repeat(const AST::RepeatStatement::Ptr &loop, bool kept_rolled,
                                  AST::Node::ListPtr &out)
{
    const std::string &fn = loop->get_filename();
    const int ln = loop->get_line();
    const AST::Node::Ptr count = loop->get_times();

    if(check_rolled(AST::to_node(loop), kept_rolled) || check_impure_calls(count)) {
        return 1;
    }

    bool count_known = false;
    unsigned long count_value = 0;
    unsigned int width = 0;
    mpz_class value;
    if(ExpressionEvaluation().evaluate_node(count, value)) {
        // §12.7.2 gives x/z a meaning (zero) but a negative count none,
        // and tools disagree on one: almost always a parameterization
        // off-by-N, so it is rejected rather than silently guessed.
        if(value < 0) {
            LOG_ERROR_N(count) << "repeat count folds to " << value << ": a loop cannot execute "
                               << "a negative number of times — a parameterization off-by-N? "
                               << "(IEEE 1800-2017 §12.7.2)";
            return 1;
        }
        if(value > mpz_class(0xFFFFFFFFUL)) {
            LOG_ERROR_N(count) << "repeat count folds to " << value << ": beyond any countdown "
                               << "the lowering will size";
            return 1;
        }
        count_known = true;
        count_value = value.convert_to<unsigned long>();
        if(count_value >= 2) {
            width = clog2(static_cast<unsigned int>(count_value));
        }
    } else {
        if(!count->is_node_type(AST::NodeType::Identifier)) {
            LOG_ERROR_N(count) << "non-constant repeat count must be a plain signal, so the "
                               << "countdown can take its declared width — bind the expression "
                               << "to a named signal first";
            return 1;
        }
        const auto &decl =
            find_declaration(m_module, AST::cast_to<AST::Identifier>(count)->get_name());
        if(!decl || declared_width(decl, width)) {
            LOG_ERROR_N(count) << "cannot size the countdown for repeat count '"
                               << AST::cast_to<AST::Identifier>(count)->get_name()
                               << "': its declaration or packed range is not resolvable";
            return 1;
        }
    }

    if(count_known && count_value == 0) {
        // §12.7.2: never runs — the loop and its body vanish.
        return 0;
    }
    if(count_known && count_value == 1) {
        // §7.2: a single pass needs no countdown, but the body still owns
        // its jumps (§8): a run-once forever, whose continues — no next
        // lap — and tail are its break.
        AST::Node::Ptr body = loop->get_statement();
        if(lower_slot(body)) {
            return 1;
        }
        rewrite_continues(body, [&fn, &ln](const AST::Node::Ptr &jump) {
            return AST::to_node(
                std::make_shared<AST::Break>(jump->get_filename(), jump->get_line()));
        });
        auto stmts = std::make_shared<AST::Node::List>();
        stmts->push_back(body);
        stmts->push_back(AST::to_node(std::make_shared<AST::Break>(fn, ln)));
        auto once = std::make_shared<AST::ForeverStatement>(fn, ln);
        once->set_statement(block_of(stmts, fn, ln));
        out->push_back(AST::to_node(once));
        return 0;
    }

    // The countdown form: load the count into the depth's shared register
    // (captured at entry, §12.7.3), test it at the head, decrement first
    // in the lap — so a continue skips nothing it should not, and the
    // entry load coalesces with the first decrement into `count - 1`.
    const unsigned int depth = m_depth;
    if(m_cnt_widths.size() <= depth) {
        m_cnt_widths.resize(depth + 1, 0);
    }
    m_cnt_widths[depth] = std::max(m_cnt_widths[depth], width);

    ++m_depth;
    AST::Node::Ptr body = loop->get_statement();
    const int body_rc = lower_slot(body);
    --m_depth;
    if(body_rc) {
        return 1;
    }

    const std::string cnt = cnt_name(depth);
    const auto load_value =
        count_known ? AST::to_node(make_const(static_cast<unsigned int>(count_value), -1, fn, ln))
                    : count->clone();
    out->push_back(make_induced_marker(make_nba(cnt, load_value, fn, ln), true, fn, ln));

    auto lap = std::make_shared<AST::Node::List>();
    lap->push_back(make_induced_marker(
        make_nba(cnt,
                 make_minus(AST::to_node(make_id(cnt, fn, ln)),
                            AST::to_node(make_const(1, static_cast<int>(width), fn, ln)), fn, ln),
                 fn, ln),
        true, fn, ln));
    lap->push_back(body);

    auto rolled = std::make_shared<AST::WhileStatement>(fn, ln);
    rolled->set_cond(make_noteq(AST::to_node(make_id(cnt, fn, ln)),
                                AST::to_node(make_const(0, static_cast<int>(width), fn, ln)), fn,
                                ln));
    rolled->set_statement(block_of(lap, fn, ln));
    out->push_back(AST::to_node(rolled));
    return 0;
}

int FsmLoopLowering::lower_for(const AST::ForStatement::Ptr &loop, bool kept_rolled,
                               AST::Node::ListPtr &out)
{
    const std::string &fn = loop->get_filename();
    const int ln = loop->get_line();

    if(check_rolled(AST::to_node(loop), kept_rolled)) {
        return 1;
    }
    const auto &pre = loop->get_pre();
    const auto &post = loop->get_post();
    const auto &cond = loop->get_cond();
    if(!pre || !post || !cond) {
        LOG_ERROR_N(loop) << "a rolled for honours the construct's full contract and "
                          << "needs all three of init, test and step";
        return 1;
    }
    const auto &pre_target = nba_like_target(pre);
    const auto &post_target = nba_like_target(post);
    if(pre_target.empty() || pre_target != post_target) {
        LOG_ERROR_N(loop) << "a rolled for's init and step assign the same plain index "
                          << "register";
        return 1;
    }
    const AST::Node::Ptr init_rhs = pre->get_right() ? pre->get_right()->get_var() : nullptr;
    const AST::Node::Ptr step_rhs = post->get_right() ? post->get_right()->get_var() : nullptr;
    if(!init_rhs || !step_rhs) {
        LOG_ERROR_N(loop) << "a rolled for's init and step carry plain expressions";
        return 1;
    }
    if(check_impure_calls(cond) || check_impure_calls(init_rhs) || check_impure_calls(step_rhs)) {
        return 1;
    }
    // The machine drives the index from its always_ff: it must exist at
    // module level as a variable, with its declared type (§7.2).
    {
        bool is_input = false;
        const auto &decl = find_declaration(m_module, pre_target, &is_input);
        if(!decl) {
            LOG_ERROR_N(cond) << "rolled for index '" << pre_target << "' is not a module-level "
                              << "declaration: the induced register takes the index's declared "
                              << "type";
            return 1;
        }
        if(is_input || !std::dynamic_pointer_cast<AST::Var>(decl)) {
            LOG_ERROR_N(cond) << "rolled for index '" << pre_target << "' is not a variable the "
                              << "machine can drive: an input port or a net cannot take the "
                              << "induced register's commits";
            return 1;
        }
    }

    AST::Node::Ptr body = loop->get_statement();
    if(lower_slot(body)) {
        return 1;
    }
    // A continue re-arms the step it would otherwise skip (§12.7.1's
    // per-iteration step), then jumps.
    const std::string index = pre_target;
    rewrite_continues(body, [&](const AST::Node::Ptr &jump) {
        auto stmts = std::make_shared<AST::Node::List>();
        stmts->push_back(make_induced_marker(
            make_nba(index, step_rhs->clone(), jump->get_filename(), jump->get_line()), true,
            jump->get_filename(), jump->get_line()));
        stmts->push_back(
            AST::to_node(std::make_shared<AST::Continue>(jump->get_filename(), jump->get_line())));
        return block_of(stmts, jump->get_filename(), jump->get_line());
    });

    out->push_back(make_induced_marker(make_nba(index, init_rhs->clone(), fn, ln), true, fn, ln));

    auto lap = std::make_shared<AST::Node::List>();
    lap->push_back(body);
    lap->push_back(make_induced_marker(make_nba(index, step_rhs->clone(), fn, ln), true, fn, ln));

    auto rolled = std::make_shared<AST::WhileStatement>(fn, ln);
    rolled->set_cond(cond->clone());
    rolled->set_statement(block_of(lap, fn, ln));
    out->push_back(AST::to_node(rolled));
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
