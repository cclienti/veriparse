// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/jump_helpers.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// True if @p node still contains a jump (Break/Continue/Return) that constant
/// control-flow folding did not consume (i.e. one still guarded by an IfStatement)
/// — committing the unrolled block would then ignore that exit.
bool has_unresolved_loop_jump(const AST::Node::Ptr &node)
{
    if(is_jump_statement(node)) {
        return true;
    }
    if(!node) {
        return false;
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(has_unresolved_loop_jump(child)) {
            return true;
        }
    }
    return false;
}

/// Erase from @p block's statement list, starting at index @p from (the region the
/// current loop iteration appended), the consumed jump and everything after it —
/// once the jump executed, those statements are unreachable (§12.8). Statements
/// before @p from belong to earlier iterations and are preserved (a consumed
/// `continue` does not truncate the iterations that follow it). When the jump sits
/// inside a nested block, the tail inside that block is erased instead.
void truncate_consumed_jump(const AST::Block::Ptr &block, std::size_t from)
{
    AST::Node::ListPtr stmts = block->get_statements();
    if(!stmts) {
        return;
    }
    auto it = stmts->begin();
    std::advance(it, std::min(from, stmts->size()));
    for(; it != stmts->end(); ++it) {
        if(is_jump_statement(*it)) {
            stmts->erase(it, stmts->end());
            return;
        }
    }
    // The jump may sit inside the last statement's nested block (execution stopped
    // there, so nothing was appended after it at this level).
    if(!stmts->empty() && stmts->back()->is_node_type(AST::NodeType::Block)) {
        truncate_consumed_jump(AST::cast_to<AST::Block>(stmts->back()), 0);
    }
}

/// Name of the variable that ultimately backs an lvalue written through a
/// bit-select/part-select chain (`f[0]`, `mem[i][j]`, …), or empty when the base
/// is not a plain identifier (e.g. a concatenation).
std::string lvalue_base_name(AST::Node::Ptr var)
{
    while(var && var->is_node_category(AST::NodeType::Indirect)) {
        var = AST::cast_to<AST::Indirect>(var)->get_var();
    }
    if(var && var->is_node_category(AST::NodeType::Identifier)) {
        return AST::cast_to<AST::Identifier>(var)->get_name();
    }
    return std::string();
}

} // namespace

VariableFolding::VariableFolding(const FunctionMap &function_map) : m_function_map(function_map) {}

void VariableFolding::set_return_target(const std::string &name) { m_return_target = name; }

void VariableFolding::set_max_unroll_iterations(unsigned long max_iterations)
{
    s_max_unroll_iterations = max_iterations;
}

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

        // Declarations without a runtime value effect.
        case AST::NodeType::Port:
        case AST::NodeType::Param:
        case AST::NodeType::TypeParam:
        case AST::NodeType::Genvar:
            return 0;

        default:
            // A statement kind the interpreter does not model (case, delays,
            // event controls, forever, …) may assign anything: unknown by
            // default, not known by default (ADR-0005 §3.1.1).
            LOG_DEBUG_N(node) << "statement not modeled by the interpreter; "
                                 "giving up constant folding from here";
            give_up();
            return 0;
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

    if(const_node) {
        LOG_DEBUG_N(subst) << "[" << lvalue_before << " = " << rvalue_before << "] evaluated to ["
                           << lvalue_str << " = "
                           << Generators::VerilogGenerator().render(const_node) << "]";
        subst->get_right()->set_var(const_node);
    }

    const auto &lvar = subst->get_left()->get_var();
    if(lvar && lvar->is_node_category(AST::NodeType::Identifier)) {
        // Whole-variable write: track the new value, or forget the variable when
        // the rvalue does not fold — never keep the stale previous value.
        const std::string name = AST::cast_to<AST::Identifier>(lvar)->get_name();
        if(const_node) {
            m_state_map[name] = const_node;
        } else {
            m_state_map.erase(name);
        }
        return 0;
    }

    // Partial-value write (bit/part-select): the rest of the base variable keeps
    // its bits but the whole-variable value is no longer known.
    const std::string base = lvalue_base_name(lvar);
    if(base.empty()) {
        give_up(); // lvalue shape not modeled (e.g. a concatenation)
        return 0;
    }
    m_state_map.erase(base);
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
    } else {
        // Either branch may run and assign anything (ADR-0005 §3.1.1).
        LOG_DEBUG_N(ifstmt) << "if condition does not fold to a constant; "
                               "giving up constant folding from here";
        give_up();
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

    // Block::replace allocates a fresh statement list on every call, so an
    // execute() that unrolls a nested loop (or folds an inner `if`) orphans any
    // list pointer held across it — dropping statements appended afterwards. So
    // re-fetch the current list (recreating one if a fold nulled it) before each
    // append.
    auto append = [&](const AST::Node::Ptr &stmt) {
        AST::Node::ListPtr stmts = block->get_statements();
        if(!stmts) {
            stmts = std::make_shared<AST::Node::List>();
            block->set_statements(stmts);
        }
        stmts->push_back(stmt);
    };

    // Statements already in the block belong to earlier iterations; a jump raised
    // below only truncates from here on.
    const std::size_t iteration_start =
        block->get_statements() ? block->get_statements()->size() : 0;

    if(body->is_node_type(AST::NodeType::Block)) {
        for(const auto &stmt : *AST::cast_to<AST::Block>(body)->get_statements()) {
            append(stmt);
            ret += execute(stmt, block);
            if(m_flow != Flow::Normal) {
                break; // the rest of this iteration is unreachable (§12.8)
            }
        }
    } else {
        append(body);
        ret += execute(body, block);
    }

    // A consumed jump and the statements it made unreachable (the tail of its own
    // branch) must not survive in the unrolled output.
    if(m_flow != Flow::Normal) {
        truncate_consumed_jump(block, iteration_start);
    }

    return ret;
}

bool VariableFolding::finalize_unrolled_loop(AST::Node::Ptr node, AST::Node::Ptr parent,
                                             AST::Block::Ptr block)
{
    // Consumed jumps were truncated as they executed; a jump still present is one
    // guarded by a non-constant condition, so the unrolling cannot be committed.
    if(has_unresolved_loop_jump(block)) {
        LOG_WARNING_N(node) << "loop body has a jump that did not fold to a constant; "
                               "leaving the loop for later lowering";
        give_up();
        return true;
    }

    pickup_statements(parent, node, block);
    return false;
}

bool VariableFolding::stop_after_iteration()
{
    if(m_flow == Flow::Returned) {
        return true; // propagate the return outward
    }
    if(m_flow == Flow::Broke) {
        m_flow = Flow::Normal; // the loop consumes the break
        return true;
    }
    m_flow = Flow::Normal; // clears a Continued signal
    return false;
}

void VariableFolding::give_up()
{
    // A construct the interpreter could not fully resolve may have assigned
    // anything: drop the whole value map (no per-construct erase list to keep
    // exhaustive) and mark the run, so nothing downstream folds to a stale value
    // and no caller certifies a constant fold (ADR-0005 §3.1.1).
    m_state_map.clear();
    m_fully_interpreted = false;
    m_flow = Flow::Normal;
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

    for(unsigned long iteration = 0;; ++iteration) {
        if(iteration >= s_max_unroll_iterations) {
            LOG_WARNING_N(node) << "for loop exceeded the unroll limit (non-terminating?); "
                                   "leaving it intact";
            give_up();
            return ret;
        }

        auto cloned = AST::cast_to<AST::ForStatement>(node->clone());

        // The condition is tested before every iteration, including the first: a
        // zero-trip loop executes nothing.
        auto cond = analyze_expression(cloned->get_cond());
        if(cond && cond->is_node_type(AST::NodeType::IntConstN)) {
            if(AST::cast_to<AST::IntConstN>(cond)->get_value() == 0) {
                break;
            }
        } else {
            LOG_WARNING_N(node) << "condition cannot be evaluated during for loop unrolling";
            give_up();
            return ret;
        }

        // Statements within for.
        ret += execute_loop_body(cloned->get_statement(), block);

        // A jump ends this iteration; `break`/`return` also end the loop, while
        // `continue` still runs the post-update and re-tests the condition.
        if(stop_after_iteration()) {
            break;
        }

        // Post update.
        auto post = cloned->get_post();
        if(post) {
            ret += execute(post, cloned);
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

        unsigned long iteration = 0;
        while(cond->get_value() != 0) {
            if(iteration++ >= s_max_unroll_iterations) {
                LOG_WARNING_N(node) << "while loop exceeded the unroll limit (only a runtime "
                                       "break exits?); leaving it intact";
                give_up();
                return ret;
            }

            // We clone the statement to analyze
            auto current = node->get_statement()->clone();
            ret += execute_loop_body(current, block);

            // `break`/`return` end the loop; `continue` just re-tests the condition.
            if(stop_after_iteration()) {
                break;
            }

            // We clone the condition and we evaluate it.
            expr = analyze_expression(node->get_cond()->clone());
            if(expr && expr->is_node_type(AST::NodeType::IntConstN)) {
                cond = AST::cast_to<AST::IntConstN>(expr);
            } else {
                LOG_WARNING_N(node) << "condition cannot be evaluated during while loop unrolling";
                give_up();
                return ret;
            }
        }

        // Commit the unrolled block, unless an unresolved break/continue forces us
        // to leave the loop for later lowering (ADR-0005 §3.2).
        finalize_unrolled_loop(node, parent, block);
    } else {
        LOG_WARNING_N(node) << "condition cannot be evaluated during while loop unrolling";
        give_up();
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
            if(stop_after_iteration()) {
                break;
            }
        }

        // Commit the unrolled block, unless an unresolved break/continue forces us
        // to leave the loop for later lowering (ADR-0005 §3.2).
        finalize_unrolled_loop(node, parent, block);
    } else {
        LOG_WARNING_N(node) << "repeat count cannot be evaluated during unrolling";
        give_up();
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

    // A task (or a call unresolved at parse time, which may be one) can write its
    // output arguments and any variable in scope (ADR-0005 §3.1.1).
    if(node->is_node_type(AST::NodeType::TaskCall) || node->is_node_type(AST::NodeType::Call)) {
        LOG_DEBUG_N(node) << "task call side effects are not modeled; "
                             "giving up constant folding from here";
        give_up();
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
