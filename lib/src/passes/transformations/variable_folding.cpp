// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>

#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// A jump statement (§12.8), bare or wrapped in a SingleStatement.
bool is_jump_statement(const AST::Node::Ptr &node)
{
    AST::Node::Ptr inner = node;
    if(inner && inner->is_node_type(AST::NodeType::SingleStatement)) {
        inner = AST::cast_to<AST::SingleStatement>(inner)->get_statement();
    }
    return inner && (inner->is_node_type(AST::NodeType::Break) ||
                     inner->is_node_type(AST::NodeType::Continue) ||
                     inner->is_node_type(AST::NodeType::Return));
}

/// Drop the jumps the interpreter actually reached: after constant-condition
/// folding a consumed break/continue/return sits unconditionally in a Block. Its
/// effect is already accounted for by the unrolling, so it must not be emitted.
/// Conditional jumps (still under an IfStatement) are left for the caller to check.
void strip_consumed_jumps(const AST::Node::Ptr &node)
{
    if(!node || !node->is_node_type(AST::NodeType::Block)) {
        return;
    }
    AST::Node::ListPtr stmts = AST::cast_to<AST::Block>(node)->get_statements();
    if(!stmts) {
        return;
    }
    for(auto it = stmts->begin(); it != stmts->end();) {
        if(is_jump_statement(*it)) {
            it = stmts->erase(it);
        } else {
            strip_consumed_jumps(*it); // descend only through unconditional blocks
            ++it;
        }
    }
}

/// True if @p node still contains a Break or Continue that constant control-flow
/// folding did not consume (i.e. one still guarded by an IfStatement) — unrolling
/// would then emit a jump outside any loop.
bool has_unresolved_loop_jump(const AST::Node::Ptr &node)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(AST::NodeType::Break) || node->is_node_type(AST::NodeType::Continue)) {
        return true;
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(has_unresolved_loop_jump(child)) {
            return true;
        }
    }
    return false;
}

/// Collect the simple identifier names assigned (blocking) anywhere in @p node.
void collect_assigned_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::BlockingSubstitution)) {
        const auto &lvalue = AST::cast_to<AST::BlockingSubstitution>(node)->get_left();
        if(lvalue && lvalue->get_var() &&
           lvalue->get_var()->is_node_category(AST::NodeType::Identifier)) {
            names.insert(AST::cast_to<AST::Identifier>(lvalue->get_var())->get_name());
        }
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        collect_assigned_names(child, names);
    }
}

} // namespace

VariableFolding::VariableFolding(const FunctionMap &function_map) : m_function_map(function_map) {}

void VariableFolding::set_return_target(const std::string &name) { m_return_target = name; }

AST::Node::Ptr VariableFolding::get_state(const std::string &var_name, bool &matched)
{
    const auto &it = m_state_map.find(var_name);
    if(it != m_state_map.end()) {
        matched = true;
        return it->second->clone();
    }

    matched = false;
    return nullptr;
}

int VariableFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(node && node->is_node_type(AST::NodeType::Initial)) {
        m_state_map.clear();
        m_flow = Flow::Normal;

        auto initial = AST::cast_to<AST::Initial>(node);
        auto ret = execute(initial->get_statement(), node);
        if(ret) {
            return ret;
        }

        // Remove empty initial
        const auto &stmt = initial->get_statement();
        if(stmt) {
            if(stmt->get_node_type() == AST::NodeType::Block) {
                const auto &block = AST::cast_to<AST::Block>(stmt);
                if(block->get_statements() == nullptr) {
                    parent->replace(node, AST::Node::Ptr(nullptr));
                }
            }
        } else {
            if(parent) {
                parent->replace(node, AST::Node::Ptr(nullptr));
            }
        }

        return 0;
    }

    return recurse_in_childs(node);
}

int VariableFolding::execute(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(node) {
        switch(node->get_node_type()) {
        case AST::NodeType::Block:
        case AST::NodeType::SingleStatement:
            return execute_in_childs(node);

        case AST::NodeType::Var:
            return execute_variable_decl(AST::cast_to<AST::Var>(node), parent);

        case AST::NodeType::BlockingSubstitution:
            return execute_blocking_substitution(AST::cast_to<AST::BlockingSubstitution>(node),
                                                 parent);

        case AST::NodeType::IfStatement:
            return execute_if(AST::cast_to<AST::IfStatement>(node), parent);

        // Jump statements (§12.8): raise the matching control-flow signal, which
        // the enclosing block / loop then acts on.
        case AST::NodeType::Return:
            return execute_return(AST::cast_to<AST::Return>(node), parent);

        case AST::NodeType::Break:
            m_flow = Flow::Broke;
            return 0;

        case AST::NodeType::Continue:
            m_flow = Flow::Continued;
            return 0;

        case AST::NodeType::ForStatement:
            return execute_for(AST::cast_to<AST::ForStatement>(node), parent);

        case AST::NodeType::WhileStatement:
            return execute_while(AST::cast_to<AST::WhileStatement>(node), parent);

        case AST::NodeType::RepeatStatement:
            return execute_repeat(AST::cast_to<AST::RepeatStatement>(node), parent);

        // A statement call may be a neutral Call (task-vs-function unresolved at
        // parse time), a FunctionCall, a TaskCall or a SystemCall.
        case AST::NodeType::Call:
        case AST::NodeType::FunctionCall:
        case AST::NodeType::TaskCall:
        case AST::NodeType::SystemCall:
            return execute_call(node, parent);
        default:
            break;
        }
    }
    return 0;
}

int VariableFolding::execute_in_childs(AST::Node::Ptr node)
{
    int ret = 0;
    AST::Node::ListPtr children = node->get_children();
    for(AST::Node::Ptr &child : *children) {
        ret += execute(child, node);
        // A jump (§12.8) makes the rest of this block/statement unreachable.
        if(m_flow != Flow::Normal) {
            break;
        }
    }
    return ret;
}

int VariableFolding::execute_variable_decl(AST::Var::Ptr var, AST::Node::Ptr parent)
{
    if(!var->get_init()) {
        return 0;
    }

    const auto &id = std::make_shared<AST::Identifier>(var->get_filename(), var->get_line());
    id->set_name(var->get_name());

    const auto &lvalue = std::make_shared<AST::Lvalue>(var->get_filename(), var->get_line());
    lvalue->set_var(id);

    const auto &subst =
        std::make_shared<AST::BlockingSubstitution>(var->get_filename(), var->get_line());
    subst->set_left(lvalue);
    subst->set_right(AST::cast_to<AST::Rvalue>(var->get_init()->clone()));

    return execute_blocking_substitution(subst, parent);
}

int VariableFolding::execute_blocking_substitution(AST::BlockingSubstitution::Ptr subst,
                                                   AST::Node::Ptr parent)
{
    std::string lvalue_before = Generators::VerilogGenerator().render(subst->get_left());
    std::string rvalue_before = Generators::VerilogGenerator().render(subst->get_right());

    std::string lvalue_str = analyze_lvalue(subst->get_left());
    AST::Node::Ptr const_node = analyze_rvalue(subst->get_right());

    if(lvalue_str.empty() || const_node == nullptr) {
        return 0;
    }

    LOG_DEBUG_N(subst) << "[" << lvalue_before << " = " << rvalue_before << "] evaluated to ["
                       << lvalue_str << " = " << Generators::VerilogGenerator().render(const_node)
                       << "]";

    m_state_map[lvalue_str] = const_node;
    subst->get_right()->set_var(const_node);
    return 0;
}

int VariableFolding::execute_if(AST::IfStatement::Ptr ifstmt, AST::Node::Ptr parent)
{
    int ret = 0;

    auto expr = analyze_expression(ifstmt->get_cond());

    if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
        ifstmt->set_cond(expr);
        auto cond = AST::cast_to<AST::IntConstN>(expr);
        AST::Node::Ptr selected_stmt;

        if(cond->get_value() != 0) {
            selected_stmt = ifstmt->get_true_statement();
        } else {
            selected_stmt = ifstmt->get_false_statement();
        }

        ret = execute(selected_stmt, ifstmt);
        pickup_statements(parent, ifstmt, selected_stmt);
    }

    return ret;
}

int VariableFolding::execute_return(AST::Return::Ptr node, AST::Node::Ptr parent)
{
    // `return expr;` sets the function result (the function-name variable, §13.4);
    // `return;` (task / void function) only signals the exit.
    if(node->get_value() && !m_return_target.empty()) {
        const auto &id = std::make_shared<AST::Identifier>(node->get_filename(), node->get_line());
        id->set_name(m_return_target);

        const auto &lvalue = std::make_shared<AST::Lvalue>(node->get_filename(), node->get_line());
        lvalue->set_var(id);

        const auto &rvalue = std::make_shared<AST::Rvalue>(node->get_filename(), node->get_line());
        rvalue->set_var(node->get_value()->clone());

        const auto &subst =
            std::make_shared<AST::BlockingSubstitution>(node->get_filename(), node->get_line());
        subst->set_left(lvalue);
        subst->set_right(rvalue);

        execute_blocking_substitution(subst, parent);
    }

    m_flow = Flow::Returned;
    return 0;
}

int VariableFolding::execute_loop_body(AST::Node::Ptr body, AST::Block::Ptr block)
{
    int ret = 0;

    // Folding an inner `if` down to nothing can leave `block` with a null list
    // (pickup_statements removed its last statement); recreate one so appends are
    // safe. Otherwise this mirrors the historical body walk exactly.
    AST::Node::ListPtr block_stmts = block->get_statements();
    if(!block_stmts) {
        block_stmts = std::make_shared<AST::Node::List>();
        block->set_statements(block_stmts);
    }

    if(body->is_node_type(AST::NodeType::Block)) {
        for(const auto &stmt : *AST::cast_to<AST::Block>(body)->get_statements()) {
            block_stmts->push_back(stmt);
            ret += execute(stmt, block);
            if(m_flow != Flow::Normal) {
                break; // the rest of this iteration is unreachable (§12.8)
            }
        }
    } else {
        block_stmts->push_back(body);
        ret += execute(body, block);
    }

    return ret;
}

bool VariableFolding::finalize_unrolled_loop(AST::Node::Ptr node, AST::Node::Ptr parent,
                                             AST::Block::Ptr block)
{
    // Remove the jumps the unrolling already accounted for; only a jump still
    // guarded by a non-constant condition should block unrolling.
    strip_consumed_jumps(block);

    if(has_unresolved_loop_jump(block)) {
        LOG_WARNING_N(node) << "loop body has a break/continue that did not fold to a constant; "
                               "leaving the loop for later lowering";
        // Drop the loop-assigned variables: their post-loop values are unknown, so
        // nothing downstream must fold to a stale constant.
        std::set<std::string> assigned;
        collect_assigned_names(node, assigned);
        for(const std::string &name : assigned) {
            m_state_map.erase(name);
        }
        m_flow = Flow::Normal;
        return true;
    }

    pickup_statements(parent, node, block);
    return false;
}

int VariableFolding::execute_for(AST::ForStatement::Ptr node, AST::Node::Ptr parent)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    // We create a temporary block to hold a temporary
    // unrolled result.
    //
    // We must pay attention that the list within the block
    // can be replaced! A new list can be created when the
    // "replace" or the "pickup_statements" methods are
    // called.
    auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

    // Pre condition.
    auto pre = node->get_pre();
    if(pre) {
        ret += execute(pre, node);
    }

    while(1) {
        auto cloned = AST::cast_to<AST::ForStatement>(node->clone());

        // Statements within for.
        ret += execute_loop_body(cloned->get_statement(), block);

        // A jump ends this iteration; `break`/`return` also end the loop, while
        // `continue` still runs the post-update and re-tests the condition.
        if(m_flow == Flow::Returned) {
            break;
        }
        if(m_flow == Flow::Broke) {
            m_flow = Flow::Normal;
            break;
        }
        m_flow = Flow::Normal; // clears a Continued signal

        // Post update.
        auto post = cloned->get_post();
        if(post) {
            ret += execute(post, cloned);
        }

        // Analyze condition.
        auto cond = analyze_expression(cloned->get_cond());
        if(cond && cond->is_node_type(AST::NodeType::IntConstN)) {
            if(AST::cast_to<AST::IntConstN>(cond)->get_value() == 0) {
                break;
            }
        } else {
            LOG_WARNING_N(node) << "condition cannot be evaluated during for loop unrolling";
            return 0;
        }
    }

    // Commit the unrolled block, unless an unresolved break/continue forces us to
    // leave the loop for later lowering (ADR-0005 §3.2).
    finalize_unrolled_loop(node, parent, block);

    return ret;
}

int VariableFolding::execute_while(AST::WhileStatement::Ptr node, AST::Node::Ptr parent)
{
    int ret = 0;

    if(!node) {
        return ret;
    }

    auto expr = analyze_expression(node->get_cond()->clone());

    if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
        auto cond = AST::cast_to<AST::IntConstN>(expr);

        // We create a temporary block to hold a temporary
        // unrolled result.
        //
        // We must pay attention that the list within the block
        // can be replaced! A new list can be created when the
        // "replace" or the "pickup_statements" methods are
        // called.
        auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

        while(cond->get_value() != 0) {
            // We clone the statement to analyze
            auto current = node->get_statement()->clone();
            ret += execute_loop_body(current, block);

            // `break`/`return` end the loop; `continue` just re-tests the condition.
            if(m_flow == Flow::Returned) {
                break;
            }
            if(m_flow == Flow::Broke) {
                m_flow = Flow::Normal;
                break;
            }
            m_flow = Flow::Normal; // clears a Continued signal

            // We clone the condition and we evaluate it.
            expr = analyze_expression(node->get_cond()->clone());
            if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
                cond = AST::cast_to<AST::IntConstN>(expr);
            } else {
                LOG_WARNING_N(node) << "condition cannot be evaluated during while loop unrolling";
                return 0;
            }
        }

        // Commit the unrolled block, unless an unresolved break/continue forces us
        // to leave the loop for later lowering (ADR-0005 §3.2).
        finalize_unrolled_loop(node, parent, block);
    }

    return ret;
}

int VariableFolding::execute_repeat(AST::RepeatStatement::Ptr node, AST::Node::Ptr parent)
{
    int ret = 0;

    auto expr = analyze_expression(node->get_times());

    if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
        // Create a temporary block to hold unrolled statements.
        //
        // We must pay attention that the list within the block
        // can be replaced! A new list can be created when the
        // "replace" or the "pickup_statements" methods are
        // called.
        auto block = std::make_shared<AST::Block>(std::make_shared<AST::Node::List>(), "");

        auto times = AST::cast_to<AST::IntConstN>(expr);
        for(mpz_class i = 0; i < times->get_value(); i++) {
            auto current = node->get_statement()->clone();
            ret += execute_loop_body(current, block);

            // `break`/`return` end the loop; `continue` moves to the next count.
            if(m_flow == Flow::Returned) {
                break;
            }
            if(m_flow == Flow::Broke) {
                m_flow = Flow::Normal;
                break;
            }
            m_flow = Flow::Normal; // clears a Continued signal
        }

        // Commit the unrolled block, unless an unresolved break/continue forces us
        // to leave the loop for later lowering (ADR-0005 §3.2).
        finalize_unrolled_loop(node, parent, block);
    }

    return ret;
}

int VariableFolding::execute_call(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    AST::Node::ListPtr args;

    // Call (neutral), FunctionCall and TaskCall all derive from Call and share
    // get_args(); SystemCall is separate.
    if(node->is_node_category(AST::NodeType::Call)) {
        args = AST::cast_to<AST::Call>(node)->get_args();
    } else if(node->is_node_type(AST::NodeType::SystemCall)) {
        args = AST::cast_to<AST::SystemCall>(node)->get_args();
    } else {
        LOG_ERROR_N(node) << "VariableFolding: unknown node type";
        return 1;
    }

    if(args) {
        for(auto arg : *args) {
            ASTReplace::replace_identifier(arg, m_state_map, node);
            auto expr = ExpressionEvaluation(m_function_map).evaluate_node(arg);
            if(expr) {
                node->replace(arg, expr);
            }
        }
    }

    return 0;
}

AST::Node::Ptr VariableFolding::analyze_rvalue(AST::Rvalue::Ptr rvalue)
{
    ASTReplace::replace_identifier(AST::to_node(rvalue), m_state_map);
    AST::Node::Ptr expr = ExpressionEvaluation(m_function_map).evaluate_node(rvalue->get_var());
    return expr;
}

std::string VariableFolding::analyze_lvalue(AST::Lvalue::Ptr lvalue)
{
    const auto &var = lvalue->get_var();

    switch(var->get_node_type()) {
    case AST::NodeType::Pointer:
        ASTReplace::replace_identifier(AST::cast_to<AST::Pointer>(var)->get_ptr(), m_state_map,
                                       var);
        break;

    case AST::NodeType::Partselect: {
        auto part = AST::cast_to<AST::Partselect>(var);
        ASTReplace::replace_identifier(part->get_msb(), m_state_map, var);
        ASTReplace::replace_identifier(part->get_lsb(), m_state_map, var);
    } break;

    case AST::NodeType::PartselectIndexed:
    case AST::NodeType::PartselectPlusIndexed:
    case AST::NodeType::PartselectMinusIndexed: {
        auto part = AST::cast_to<AST::PartselectIndexed>(var);
        ASTReplace::replace_identifier(part->get_index(), m_state_map, var);
        ASTReplace::replace_identifier(part->get_size(), m_state_map, var);
    } break;

    default:
        break;
    }

    ConstantFolding().run(AST::to_node(lvalue));
    return Generators::VerilogGenerator().render(lvalue->get_var());
}

AST::Node::Ptr VariableFolding::analyze_expression(AST::Node::Ptr expr)
{
    auto rvalue = std::make_shared<AST::Rvalue>(expr);
    return analyze_rvalue(rvalue);
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
