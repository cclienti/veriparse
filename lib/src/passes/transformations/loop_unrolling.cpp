// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>

#include <iterator>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{
using Range = std::pair<std::string, std::vector<AST::Node::Ptr>>;

std::string print_for_range(const Range &for_range)
{
    std::stringstream ss;
    ss << "{name: " << for_range.first << ", range: [";
    for(uint64_t i = 0; i < for_range.second.size(); ++i) {
        if((i < 4) || (i == for_range.second.size() - 1)) {
            const auto &v = for_range.second[i];
            if(v->is_node_type(AST::NodeType::FloatConst)) {
                ss << AST::cast_to<AST::FloatConst>(v)->get_value();
            }
            if(v->is_node_type(AST::NodeType::IntConstN)) {
                ss << AST::cast_to<AST::IntConstN>(v)->get_value();
            }
            if(i < for_range.second.size() - 1) {
                ss << ", ";
            }
        } else if(i == 4) {
            ss << "... ";
        }
    }
    ss << "]}";

    return ss.str();
}

/// True if @p node contains a Break/Continue (§12.8) that belongs to THIS loop —
/// i.e. not one nested inside another loop, which owns its own jumps.
bool body_owns_jump(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::Break) || node->is_node_type(AST::NodeType::Continue)) {
        return true;
    }
    // A nested loop scopes its own break/continue; do not look inside it.
    if(node->is_node_type(AST::NodeType::ForStatement) ||
       node->is_node_type(AST::NodeType::WhileStatement) ||
       node->is_node_type(AST::NodeType::RepeatStatement) ||
       node->is_node_type(AST::NodeType::ForeverStatement)) {
        return false;
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(body_owns_jump(child)) {
            return true;
        }
    }
    return false;
}

/// Is @p n (or the statement it wraps) a jump of node type @p jt (Break/Continue)?
bool is_jump_of(const AST::Node::Ptr &n, AST::NodeType jt)
{
    AST::Node::Ptr inner = n;
    if(inner && inner->is_node_type(AST::NodeType::SingleStatement)) {
        inner = AST::cast_to<AST::SingleStatement>(inner)->get_statement();
    }
    return inner && inner->is_node_type(jt);
}

/// True if @p node contains a jump of type @p jt owned by this loop (not one in a
/// nested loop).
bool owns_jump_of(const AST::Node::Ptr &node, AST::NodeType jt)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(jt)) {
        return true;
    }
    if(node->is_node_type(AST::NodeType::ForStatement) ||
       node->is_node_type(AST::NodeType::WhileStatement) ||
       node->is_node_type(AST::NodeType::RepeatStatement) ||
       node->is_node_type(AST::NodeType::ForeverStatement)) {
        return false;
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(owns_jump_of(child, jt)) {
            return true;
        }
    }
    return false;
}

/// If @p s is exactly `if (cond) <jump jt>;` (no else, the then-branch is just the
/// jump), return its condition; otherwise null. This is the shape the structural
/// lowering can turn into a guard.
AST::Node::Ptr conditional_jump_cond(const AST::Node::Ptr &s, AST::NodeType jt)
{
    if(!s->is_node_type(AST::NodeType::IfStatement)) {
        return nullptr;
    }
    const auto &ifs = AST::cast_to<AST::IfStatement>(s);
    if(ifs->get_false_statement()) {
        return nullptr;
    }
    if(is_jump_of(ifs->get_true_statement(), jt)) {
        return ifs->get_cond();
    }
    return nullptr;
}

/// Build `if (cond) begin end else begin <stmts> end` — the jump's condition kept
/// as-is, the remainder moved to the else branch. This preserves the source's
/// X-propagation: like `if (cond) jump;`, the remainder runs whenever the jump is
/// not taken, including when `cond` is X (a negated guard `if (!cond)` would wrongly
/// skip the remainder on X). Stays synthesizable (no `!==`).
AST::Node::Ptr make_guard_if(const AST::Node::Ptr &cond, AST::Node::ListPtr stmts)
{
    const std::string fn = cond->get_filename();
    const int ln = cond->get_line();
    const auto &ifs = std::make_shared<AST::IfStatement>(fn, ln);
    ifs->set_cond(cond->clone());
    ifs->set_true_statement(std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), ""));
    ifs->set_false_statement(std::make_shared<AST::Block>(stmts, ""));
    return ifs;
}

/// Lower one jump kind (@p jt) in a statement sequence to structural guards, with
/// no flag variable: `if (c) jump; rest` becomes `if (!c) begin rest end`, and a
/// bare `jump;` drops the rest. @p ok is cleared if a jump appears in a shape too
/// complex to lower this way (nested in a block or a deeper `if`), so the caller
/// can leave the loop intact instead.
AST::Node::ListPtr lower_jump_seq(AST::Node::ListPtr stmts, AST::NodeType jt, bool &ok)
{
    const auto &result = std::make_shared<AST::Node::List>();
    for(auto it = stmts->begin(); it != stmts->end(); ++it) {
        const AST::Node::Ptr &s = *it;

        const AST::Node::Ptr cond = conditional_jump_cond(s, jt);
        if(cond) {
            const auto &rest = std::make_shared<AST::Node::List>(std::next(it), stmts->end());
            const auto &lowered_rest = lower_jump_seq(rest, jt, ok);
            if(!ok) {
                return result;
            }
            if(!lowered_rest->empty()) {
                result->push_back(make_guard_if(cond, lowered_rest));
            }
            return result; // the remainder now lives inside the guard
        }

        if(is_jump_of(s, jt)) {
            return result; // unconditional jump: the rest is unreachable
        }

        if(owns_jump_of(s, jt)) {
            ok = false; // a jump too deeply nested for the flag-free form
            return result;
        }

        result->push_back(s);
    }
    return result;
}
} // namespace

LoopUnrolling::LoopUnrolling(const FunctionMap &function_map) : m_function_map(function_map) {}

int LoopUnrolling::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    int ret;

    m_scope_map.clear();

    ret = Analysis::UniqueDeclaration::analyze(node, m_scope_declared);
    if(ret) {
        return ret;
    }

    ret = unroll(node, parent, "");
    if(ret) {
        return ret;
    }

    ret = rename_scoped_identifiers(node, parent);
    if(ret) {
        return ret;
    }

    return 0;
}

int LoopUnrolling::unroll(AST::Node::Ptr node, AST::Node::Ptr parent, const std::string scope_state)
{
    if(!node) {
        return 0;
    }

    if(node->is_node_type(AST::NodeType::ForStatement)) {
        const AST::ForStatement::Ptr for_node = AST::cast_to<AST::ForStatement>(node);

        if(!parent) {
            LOG_ERROR_N(for_node) << "could not unroll loop, no parent";
            return 1;
        }

        // Jump lowering (ADR-0005 §3.2). A break/continue cannot simply be cloned —
        // it would land outside any loop. `continue` is lowered into the body now
        // (each iteration guards its own remainder); `break` is lowered across the
        // flat unrolled list afterwards (it also skips later iterations). Mixed
        // break+continue, or a jump nested too deeply for the flag-free guard form,
        // is left intact for the downstream tool.
        bool lower_break_after = false;
        if(body_owns_jump(for_node->get_statement())) {
            const bool has_break = owns_jump_of(for_node->get_statement(), AST::NodeType::Break);
            const bool has_continue =
                owns_jump_of(for_node->get_statement(), AST::NodeType::Continue);

            AST::Node::ListPtr body_stmts;
            std::string body_scope;
            if(for_node->get_statement()->is_node_type(AST::NodeType::Block)) {
                const auto &b = AST::cast_to<AST::Block>(for_node->get_statement());
                body_stmts = b->get_statements();
                body_scope = b->get_scope();
            } else {
                body_stmts = std::make_shared<AST::Node::List>();
                body_stmts->push_back(for_node->get_statement());
            }

            bool ok = !(has_break && has_continue); // mixed break+continue: not handled
            if(ok) {
                const AST::NodeType jt = has_break ? AST::NodeType::Break : AST::NodeType::Continue;
                const auto &lowered = lower_jump_seq(body_stmts, jt, ok);
                if(ok && has_continue) {
                    // Replace the body with its continue-free form and unroll normally.
                    for_node->set_statement(std::make_shared<AST::Block>(lowered, body_scope));
                }
            }
            if(!ok) {
                LOG_WARNING_N(for_node) << "for loop uses break/continue in a form that cannot be "
                                           "unrolled; leaving it intact";
                return 0;
            }
            lower_break_after = has_break;
        }

        // Get the loop scope
        std::string scope;
        if(for_node->get_statement()->is_node_type(AST::NodeType::Block)) {
            const auto &block = AST::cast_to<AST::Block>(for_node->get_statement());
            scope = block->get_scope();
            LOG_TRACE_N(for_node) << "loop scope: " << scope;
        } else {
            LOG_WARNING_N(node) << "for loop without named scope";
        }

        if(scope.empty()) {
            LOG_WARNING_N(node) << "for loop without named scope";
            scope = Analysis::UniqueDeclaration::get_unique_identifier(scope, m_scope_declared);
        }

        // Look for the current for loop range
        LoopUnrolling::RangePtr for_range = LoopUnrolling::get_for_range(for_node);
        if(for_range) {
            LOG_TRACE_N(for_node) << print_for_range(*for_range);

            // Unroll the statements
            AST::Node::ListPtr stmts;
            if(for_node->get_statement()->is_node_type(AST::NodeType::Block)) {
                stmts = AST::cast_to<AST::Block>(for_node->get_statement())->get_statements();
            } else {
                stmts = std::make_shared<AST::Node::List>();
                stmts->push_back(for_node->get_statement());
            }

            // Replace the loop identifier in unrolled statements and annotate declarations
            AST::Node::ListPtr unrolled_stmts = std::make_shared<AST::Node::List>();
            AnnotateDeclaration annotate;

            for(AST::Node::Ptr p : for_range->second) {
                auto stmts_copy = AST::Node::clone_list(stmts);

                ASTReplace::replace_identifier(stmts_copy, for_range->first, p);

                // Manage the scope mapping
                const std::string loop_value = Generators::VerilogGenerator().render(p);
                const std::string src_scope = scope + "[" + loop_value + "]";
                const std::string dest_scope = for_range->first + "_" + loop_value;
                const std::string new_scope_state = scope_state + src_scope + ".";

                if(map_scope(new_scope_state, scope_state, dest_scope)) {
                    LOG_ERROR_N(node) << "error during scope mapping";
                    return 1;
                }

                // We put the stmts vector in a AST::Block in order to
                // annotate all stmts at a time.
                const std::string replace = std::string("$&_") + dest_scope;
                annotate.set_search_replace("^.*$", replace);
                AST::Block::Ptr block = std::make_shared<AST::Block>(stmts_copy, "");
                annotate.run(block);

                // We pushed the new copied stmts into a block
                auto block_tmp = std::make_shared<AST::Block>(stmts_copy, src_scope);

                // We recurse using the block_tmp as parent. This block is
                // used to hold recurse results.
                auto recurse_fct = [this, &new_scope_state](AST::Node::Ptr n, AST::Node::Ptr p) {
                    return unroll(n, p, new_scope_state);
                };
                if(recurse(block_tmp, stmts_copy, recurse_fct)) {
                    return 1;
                }

                // We accumulate all results
                unrolled_stmts->splice(unrolled_stmts->end(), *block_tmp->get_statements());
            }

            // Lower `break` across the whole unrolled sequence: it skips the rest of
            // its own iteration and every later one (ADR-0005 §3.2).
            if(lower_break_after) {
                bool ok = true;
                unrolled_stmts = lower_jump_seq(unrolled_stmts, AST::NodeType::Break, ok);
            }

            // Replace the all unrolled statements in the true parent
            // block.
            pickup_statements(parent, node, unrolled_stmts);

            return 0;
        }
    }

    else if(node->is_node_type(AST::NodeType::RepeatStatement)) {
        const AST::RepeatStatement::Ptr repeat_node = AST::cast_to<AST::RepeatStatement>(node);

        if(!parent) {
            LOG_ERROR_N(repeat_node) << "could not unroll loop, no parent";
            return 1;
        }

        // See the for-loop note above: a break/continue can't be unrolled yet.
        if(body_owns_jump(repeat_node->get_statement())) {
            LOG_WARNING_N(repeat_node) << "repeat loop uses break/continue; leaving it un-unrolled "
                                          "(jump lowering pending)";
            return 0;
        }

        // Get the loop scope
        std::string scope;
        if(repeat_node->get_statement()->is_node_type(AST::NodeType::Block)) {
            const auto &block = AST::cast_to<AST::Block>(repeat_node->get_statement());
            scope = block->get_scope();
            LOG_TRACE_N(repeat_node) << "loop scope: " << scope;
        } else {
            LOG_WARNING_N(node) << "for loop without named scope";
        }

        if(scope.empty()) {
            LOG_WARNING_N(node) << "for loop without named scope";
            scope = Analysis::UniqueDeclaration::get_unique_identifier(scope, m_scope_declared);
        }

        AST::Node::Ptr times_node =
            ExpressionEvaluation(m_function_map).evaluate_node(repeat_node->get_times());
        if(times_node && times_node->is_node_type(AST::NodeType::IntConstN)) {
            AST::IntConstN::Ptr times = AST::cast_to<AST::IntConstN>(times_node);

            // Unroll the statements
            AST::Node::ListPtr stmts;
            if(repeat_node->get_statement()->is_node_type(AST::NodeType::Block)) {
                stmts = AST::cast_to<AST::Block>(repeat_node->get_statement())->get_statements();
            } else {
                stmts = std::make_shared<AST::Node::List>();
                stmts->push_back(repeat_node->get_statement());
            }

            AST::Node::ListPtr unrolled_stmts = std::make_shared<AST::Node::List>();
            for(uint64_t x = 0; x < times->get_value().convert_to<unsigned long>(); x++) {
                AST::Node::ListPtr stmts_copy = AST::Node::clone_list(stmts);

                // Manage the scope mapping
                const std::string loop_value = std::to_string(x);
                const std::string src_scope = scope + "[" + loop_value + "]";
                const std::string dest_scope = loop_value;
                const std::string new_scope_state = scope_state + src_scope + ".";

                if(map_scope(new_scope_state, scope_state, dest_scope)) {
                    LOG_ERROR_N(node) << "error during scope mapping";
                    return 1;
                }

                // We pushed the new copied stmts into a block
                auto block_tmp = std::make_shared<AST::Block>(stmts_copy, src_scope);

                // We recurse using the block_tmp as parent. this block is used to hold recurse
                // results.
                auto recurse_fct = [this, &new_scope_state](AST::Node::Ptr n, AST::Node::Ptr p) {
                    return unroll(n, p, new_scope_state);
                };
                if(recurse(block_tmp, stmts_copy, recurse_fct)) {
                    return 1;
                }

                unrolled_stmts->splice(unrolled_stmts->end(), *stmts_copy);
            }

            // Replace the unrolled statements in the parent block.
            pickup_statements(parent, node, unrolled_stmts);

            return 0;
        } else {
            LOG_WARNING_N(node) << "non integer repeat";
        }
    }

    std::string new_scope_state = scope_state;
    if(node->is_node_type(AST::NodeType::Block)) {
        const auto &block = AST::cast_to<AST::Block>(node);

        std::string scope = block->get_scope();
        if(scope.empty()) {
            scope = Analysis::UniqueDeclaration::get_unique_identifier(scope, m_scope_declared);
        }

        new_scope_state.append(scope + ".");
        if(map_scope(new_scope_state, scope_state, "")) {
            LOG_ERROR_N(node) << "error during scope mapping";
            return 1;
        }
    }

    auto recurse_fct = [this, &new_scope_state](AST::Node::Ptr n, AST::Node::Ptr p) {
        return unroll(n, p, new_scope_state);
    };
    return recurse_in_childs(node, recurse_fct);
}

int LoopUnrolling::map_scope(const std::string &verilog_scope, const std::string &scope_state,
                             const std::string &rename_suffix)
{
    // Find scope mapping of the current state.
    std::string rename_suffix_prev;
    if(!scope_state.empty()) {
        auto itfind = m_scope_map.find(scope_state);
        if(itfind != m_scope_map.end()) {
            rename_suffix_prev = itfind->second;
        } else {
            LOG_ERROR << "scope " << scope_state << " not found";
            return 1;
        }
    }

    // Append the current dest mapping to the previous one.
    const std::string rename_suffix_mapping = rename_suffix_prev + "_" + rename_suffix;

    // Insert the current mapping into the scope map.
    auto map_res = m_scope_map.emplace(verilog_scope, rename_suffix_mapping);
    if(!map_res.second) {
        LOG_ERROR << "scope already defined: " << verilog_scope;
        return 1;
    }

    return 0;
}

int LoopUnrolling::rename_scoped_identifiers(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    // Check for defparam first
    if(node->is_node_type(AST::NodeType::Defparam)) {
        const auto &defparam = AST::cast_to<AST::Defparam>(node);
        const auto &identifier = defparam->get_identifier();
        const auto &scope = identifier->get_hier();
        if(scope) {
            const auto &labellist = scope->get_labellist();
            if(labellist && labellist->size() >= 2) {
                const auto scope_str = Generators::VerilogGenerator().render(labellist->front());
                auto itfind = m_scope_map.find(scope_str);
                if(itfind != m_scope_map.end()) {
                    labellist->pop_front();
                    const auto new_name = labellist->front()->get_name() + itfind->second;
                    labellist->front()->set_name(new_name);
                }
            }
        }
        // nothing more to recurse
        return 0;
    }
    // Else check if identifier
    else if(node->is_node_type(AST::NodeType::Identifier)) {
        const auto &identifier = AST::cast_to<AST::Identifier>(node);
        const auto &scope = identifier->get_hier();
        if(scope) {
            const auto scope_str = Generators::VerilogGenerator().render(scope);
            auto itfind = m_scope_map.find(scope_str);
            if(itfind != m_scope_map.end()) {
                identifier->set_hier(nullptr);
                const auto new_name = identifier->get_name() + itfind->second;
                identifier->set_name(new_name);
            } else {
                LOG_ERROR_N(node) << "scope " << scope_str << " not found";
                return 1;
            }
        }
        // nothing more to recurse
        return 0;
    }

    auto recurse_fct = [this](AST::Node::Ptr n, AST::Node::Ptr p) {
        return rename_scoped_identifiers(n, p);
    };
    return recurse_in_childs(node, recurse_fct);
}

LoopUnrolling::RangePtr LoopUnrolling::get_for_range(const AST::ForStatement::Ptr &for_node)
{
    const AST::BlockingSubstitution::Ptr pre_subst = for_node->get_pre();
    if(!pre_subst) {
        LOG_WARNING_N(for_node) << "No pre-condition found, could not unroll loop";
        return nullptr;
    }

    const AST::BlockingSubstitution::Ptr post_subst = for_node->get_post();
    if(!post_subst) {
        LOG_WARNING_N(for_node) << "No post-condition found, could not unroll loop";
        return nullptr;
    }

    AST::Node::Ptr loop_cond = for_node->get_cond();
    if(!loop_cond) {
        LOG_WARNING_N(for_node) << "No condition found, could not unroll loop";
        return nullptr;
    }

    const std::string loop_pre_lvalue = LoopUnrolling::get_cond_lvalue(pre_subst);
    if(!loop_pre_lvalue.size()) {
        return nullptr;
    }

    const std::string loop_post_lvalue = LoopUnrolling::get_cond_lvalue(post_subst);
    if(!loop_post_lvalue.size()) {
        return nullptr;
    }

    if(loop_post_lvalue != loop_pre_lvalue) {
        LOG_WARNING_N(for_node) << "pre-condition and post-condition lvalues are different, "
                                << "could not unroll loop";
        return nullptr;
    }

    AST::Node::Ptr loop_pre_rvalue = ExpressionEvaluation(m_function_map)
                                         .evaluate_node(LoopUnrolling::get_cond_rvalue(pre_subst));
    if(!loop_pre_rvalue) {
        LOG_WARNING_N(for_node) << "pre-condition rvalue is not constant, could not unroll loop";
        return nullptr;
    }

    AST::Node::Ptr loop_post_rvalue = LoopUnrolling::get_cond_rvalue(post_subst);
    if(!loop_post_rvalue) {
        LOG_WARNING_N(for_node) << "no post condition given, could not unroll loop";
        return nullptr;
    }

    ExpressionEvaluation::ReplaceMap replace_map;
    replace_map[loop_pre_lvalue] = AST::cast_to<AST::Constant>(loop_pre_rvalue);

    RangePtr range = std::make_shared<Range>();
    range->first = loop_pre_lvalue;

    while(true) {
        AST::Node::Ptr cond_eval =
            ExpressionEvaluation(replace_map, m_function_map).evaluate_node(loop_cond);
        if(!cond_eval) {
            LOG_WARNING_N(for_node) << "could not evaluate condition, loop not unrolled";
            return nullptr;
        }

        bool cond;

        if(cond_eval->is_node_type(AST::NodeType::FloatConst)) {
            AST::FloatConst::Ptr fconst = AST::cast_to<AST::FloatConst>(cond_eval);
            cond = fconst->get_value() != 0;
        } else if(cond_eval->is_node_type(AST::NodeType::IntConstN)) {
            AST::IntConstN::Ptr iconst = AST::cast_to<AST::IntConstN>(cond_eval);
            cond = iconst->get_value() != 0;
        } else {
            LOG_WARNING_N(for_node) << "could not evaluate condition, loop not unrolled";
            return nullptr;
        }

        if(cond) {
            range->second.push_back(replace_map[loop_pre_lvalue]);
            AST::Node::Ptr eval =
                ExpressionEvaluation(replace_map, m_function_map).evaluate_node(loop_post_rvalue);
            if(!eval) {
                LOG_WARNING_N(for_node) << "could not evaluate post-condition, loop not unrolled";
                return nullptr;
            }
            if(eval->is_node_category(AST::NodeType::Constant)) {
                replace_map[loop_pre_lvalue] = AST::cast_to<AST::Constant>(eval);
            } else {
                LOG_WARNING_N(for_node)
                    << "post-condition evaluation is not constant, loop not unrolled";
                return nullptr;
            }
        } else {
            break;
        }
    }
    return range;
}

std::string LoopUnrolling::get_cond_lvalue(const AST::BlockingSubstitution::Ptr &subst)
{
    const AST::Lvalue::Ptr lvalue = subst->get_left();
    if(!lvalue) {
        return "";
    }

    AST::Node::Ptr identifier = AST::cast_to<AST::Lvalue>(lvalue)->get_var();
    if(!identifier) {
        return "";
    }
    if(!identifier->is_node_category(AST::NodeType::Identifier)) {
        return "";
    }

    return AST::cast_to<AST::Identifier>(identifier)->get_name();
}

AST::Node::Ptr LoopUnrolling::get_cond_rvalue(const AST::BlockingSubstitution::Ptr &subst)
{
    const AST::Rvalue::Ptr rvalue = subst->get_right();
    if(!rvalue) {
        LOG_WARNING_N(subst) << "Rvalue is empty";
        return nullptr;
    }
    return rvalue->get_var();
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
