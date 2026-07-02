// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING
#define VERIPARSE_PASSES_TRANSFORMATIONS_LOOP_UNROLLING

#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/AST/nodes.hpp>

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

class LoopUnrolling : public TransformationBase
{
private:
    using Range = std::pair<std::string, std::vector<AST::Node::Ptr>>;
    using RangePtr = std::shared_ptr<Range>;
    using ScopeMap = std::map<std::string, std::string>;
    using FunctionMap = Analysis::Module::FunctionMap;

public:
    LoopUnrolling() = default;

    LoopUnrolling(const FunctionMap &function_map);

private:
    /**
     * @brief Apply the LoopUnrolling transform.
     *
     * @return zero on success.
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Unroll loops
     *
     * Unroll loops and keep a track of scope and how identifiers are
     * renamed.
     *
     * @return zero on success.
     */
    int unroll(AST::Node::Ptr node, AST::Node::Ptr parent, const std::string scope_state);

    /**
     * @brief Prepare a loop body that uses break/continue for unrolling (§3.2),
     * shared by the for and repeat branches.
     *
     * @param body   the loop's statement.
     * @param loop   the loop node, for diagnostics.
     * @param lower_break_after  set true when a `break` must be lowered across the
     *                           flat unrolled sequence afterwards.
     * @param new_body  set to a rewritten (continue-lowered) body the caller should
     *                  install, or null when the body is unchanged.
     * @return 0 to proceed with unrolling, 1 to leave the loop intact.
     */
    int analyze_loop_jumps(const AST::Node::Ptr &body, const AST::Node::Ptr &loop,
                           bool &lower_break_after, AST::Node::Ptr &new_body);

    /**
     * @brief Scope name of a loop body: the body block's own scope, or a generated
     * unique one when the body is not a named block.
     */
    std::string loop_scope(const AST::Node::Ptr &body, const AST::Node::Ptr &loop);

    /**
     * @brief Unroll one loop iteration, shared by the for and repeat branches: map
     * @p src_scope to @p dest_scope, recurse into @p stmts (nested loops unroll
     * here), and append the results to @p unrolled_stmts.
     *
     * @return zero on success.
     */
    int unroll_iteration(const AST::Node::Ptr &loop, const AST::Node::ListPtr &stmts,
                         const std::string &src_scope, const std::string &scope_state,
                         const std::string &dest_scope, const AST::Node::ListPtr &unrolled_stmts);

    /**
     * @brief Install the unrolled statements in place of @p loop, lowering any
     * pending `break` across the flat sequence first (ADR-0005 §3.2). Fails — and
     * installs nothing — if the lowering cannot complete.
     *
     * @return zero on success.
     */
    int install_unrolled(const AST::Node::Ptr &parent, const AST::Node::Ptr &loop,
                         AST::Node::ListPtr unrolled_stmts, bool lower_break_after);

    /**
     * @brief Rename scoped identifier with mapping gathered during
     * unrolling.
     *
     * @return zero on success.
     */
    int rename_scoped_identifiers(AST::Node::Ptr node, AST::Node::Ptr parent);

    /**
     * @brief Push scope into map of scopes.
     *
     * The scope of map keep tracks of how identifiers are renamed
     * depending on scope hierarchy.
     *
     * @return zero on success.
     */
    int map_scope(const std::string &verilog_scope, const std::string &scope_state,
                  const std::string &rename_suffix);

    /**
     * @brief Evaluate a ForStatement Node.
     *
     * It returns a range_t made of the loop variable string and the
     * vector of all loop index values. It returns nullptr if the pre,
     * post or condition part of the for expression cannot be resolved
     * statically.
     *
     * @return The range pointer.
     */
    RangePtr get_for_range(const AST::ForStatement::Ptr &for_node);

    /**
     * @brief Return the lvalue identifier name of a blocking
     * substitution.
     *
     * @return non-empty string on success.
     */
    std::string get_cond_lvalue(const AST::BlockingSubstitution::Ptr &subst);

    /**
     * @brief Return the rvalue node of a blocking substitution.
     *
     * @return non-nullptr on success.
     */
    AST::Node::Ptr get_cond_rvalue(const AST::BlockingSubstitution::Ptr &subst);

private:
    Analysis::UniqueDeclaration::IdentifierSet m_scope_declared;
    ScopeMap m_scope_map;
    FunctionMap m_function_map;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
