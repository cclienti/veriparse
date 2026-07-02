// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING
#define VERIPARSE_PASSES_TRANSFORMATIONS_VARIABLE_FOLDING

#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/AST/nodes.hpp>
#include <string>
#include <map>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

class VariableFolding : public TransformationBase
{
public:
    using StateMap = std::map<std::string, AST::Node::Ptr>;
    using FunctionMap = Analysis::Module::FunctionMap;

    VariableFolding() = default;

    VariableFolding(const FunctionMap &function_map);

    AST::Node::Ptr get_state(const std::string &var_name, bool &matched);

    /**
     * @brief Name of the variable a `return expr;` assigns to (the function-name
     * variable, IEEE 1800-2017 §13.4). Empty means `return` carries no value here
     * (a task / void function) — it only signals the early exit.
     */
    void set_return_target(const std::string &name);

    /**
     * @brief True if every statement the interpreter walked was completely
     * modeled. A construct it could not resolve to concrete control flow and
     * concrete values makes the run not fully interpreted — its results can be
     * *used* conservatively but must not certify a constant fold (ADR-0005
     * §3.1.1).
     */
    bool is_fully_interpreted() const { return m_fully_interpreted; }

    /**
     * @brief Cap on unrolled loop iterations. A loop with no compile-time-constant
     * termination (e.g. `while (1)` whose only exit is a runtime `break`, §12.8)
     * would otherwise unroll without bound; past the cap the loop is left intact
     * and the interpreter gives up (§3.1.1).
     */
    static void set_max_unroll_iterations(unsigned long max_iterations);

private:
    /**
     * @brief Non-local control-flow state produced by a jump statement (§12.8),
     * threaded through statement evaluation. A compound statement stops evaluating
     * its remaining children once this leaves Normal; a loop consumes Broke /
     * Continued, and Returned propagates out to the top.
     */
    enum class Flow
    {
        Normal,
        Returned,
        Broke,
        Continued
    };

    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Walk through all elements of a block. For each
     * node, it maintains a state map of variables
     * values. If possible, all rvalues will be replaced by
     * their results.
     *
     * @return zero on success
     */
    virtual int execute(AST::Node::Ptr node, AST::Node::Ptr parent);

    /**
     * @brief Walk through a block.
     *
     * @return zero on success
     */
    virtual int execute_in_childs(AST::Node::Ptr node);

    /**
     * @brief Walk through a variable declaration.
     *
     * @return zero on success
     */
    virtual int execute_variable_decl(AST::Var::Ptr var);

    /**
     * @brief Walk through a blocking substitution.
     *
     * @return zero on success
     */
    virtual int execute_blocking_substitution(AST::BlockingSubstitution::Ptr subst);

    /**
     * @brief Walk through a if statement.
     *
     * @return zero on success
     */
    virtual int execute_if(AST::IfStatement::Ptr ifstmt, AST::Node::Ptr parent);

    /**
     * @brief Evaluate a `return [expr];` (§12.8): assign `expr` to the return
     * target (when both are present), then raise the Returned flow so the enclosing
     * statements stop executing.
     *
     * @return zero on success
     */
    virtual int execute_return(AST::Return::Ptr node);

    /**
     * @brief Execute one unrolled loop iteration's body into @p block, appending
     * each executed statement. Stops at the first statement that raises a jump
     * (§12.8) and truncates the consumed jump together with the statements it made
     * unreachable — they must not survive in the unrolled output.
     *
     * @return zero on success
     */
    virtual int execute_loop_body(AST::Node::Ptr body, AST::Block::Ptr block);

    /**
     * @brief Commit an unrolled loop, or leave it intact. If the unrolled @p block
     * still holds a jump that did not fold to a constant, the loop cannot be
     * safely unrolled (the jump would land outside any loop); the original loop is
     * kept for LoopUnrolling (ADR-0005 §3.2) and the interpreter gives up
     * (§3.1.1).
     *
     * @return true if the loop was left intact (not unrolled)
     */
    virtual bool finalize_unrolled_loop(AST::Node::Ptr node, AST::Node::Ptr parent,
                                        AST::Block::Ptr block);

    /**
     * @brief React to a construct the interpreter could not fully resolve: drop
     * the whole state map (its unknown effects may have touched anything) and mark
     * the run not fully interpreted (ADR-0005 §3.1.1).
     */
    void give_up();

    /**
     * @brief Act on the control-flow signal left by an unrolled iteration and say
     * whether the loop stops: Returned stops it and propagates outward; Broke stops
     * it and is consumed; Continued/Normal continue (Continued is consumed).
     *
     * @return true if the enclosing loop must stop iterating
     */
    bool stop_after_iteration();

    /**
     * @brief Check the unroll cap before an iteration; past it the interpreter
     * gives up (§3.1.1) so the loop is left intact.
     *
     * @return true if the cap is reached and the loop must be abandoned
     */
    bool loop_cap_reached(unsigned long iteration, const AST::Node::Ptr &node);

    /**
     * @brief Fold a loop-control expression (condition or repeat count) to a
     * constant. When it does not fold, the interpreter gives up (§3.1.1) — @p what
     * names the expression in the diagnostic.
     *
     * @return the constant, or nullptr when the expression did not fold
     */
    AST::IntConstN::Ptr analyze_loop_constant(const AST::Node::Ptr &expr,
                                              const AST::Node::Ptr &node, const char *what);

    /**
     * @brief Walk through a for statement.
     *
     * @return zero on success
     */
    virtual int execute_for(AST::ForStatement::Ptr forstmt, AST::Node::Ptr parent);

    /**
     * @brief Walk through a while statement.
     *
     * @return zero on success
     */
    virtual int execute_while(AST::WhileStatement::Ptr whilestmt, AST::Node::Ptr parent);

    /**
     * @brief Walk through a repeat statement.
     *
     * @return zero on success
     */
    virtual int execute_repeat(AST::RepeatStatement::Ptr repeatstmt, AST::Node::Ptr parent);

    /**
     * @brief Walk through a call statement. A function/system call has its
     * arguments folded; a task or parse-time-unresolved call makes the
     * interpreter give up (§3.1.1) with its arguments left untouched — an actual
     * may be an output writeback target.
     *
     * @return zero on success
     */
    virtual int execute_call(AST::Node::Ptr call);

    /**
     * @brief Resolve the rvalue using the variables
     * state. Return the constant value. The given rvalue is
     * updated with the node created using the rvalue result.
     *
     * @return constant value on success, else nullptr
     */
    virtual AST::Node::Ptr analyze_rvalue(AST::Rvalue::Ptr rvalue);

    /**
     * @brief Resolve in place the indices of a pointer/part-select lvalue using
     * the current variable states, then constant-fold them.
     *
     * @return the rendered lvalue (diagnostics only)
     */
    virtual std::string fold_lvalue_indices(AST::Lvalue::Ptr lvalue);

    /**
     * @brief Resolve the expression using the variables
     * state. Return the constant value.
     *
     * @return constant value on success, else nullptr
     */
    virtual AST::Node::Ptr analyze_expression(AST::Node::Ptr expr);

private:
    StateMap m_state_map;
    FunctionMap m_function_map;
    Flow m_flow = Flow::Normal;
    std::string m_return_target;
    bool m_fully_interpreted = true;

    static inline unsigned long s_max_unroll_iterations = 100000ul;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
