// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_FSM_TASK_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_FSM_TASK_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <map>
#include <set>
#include <string>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Inline every task call of the (* veriparse_fsm *) processes
 * (ADR-0014 §7.4) — the structural pre-lowering ImplicitFsmElaboration
 * consumes.
 *
 * Per call site the task body becomes one labelled block, its formals
 * locals: an input copies in at the call, an output or inout copies out
 * at the return, following measured IEEE §13.3 semantics; a reference
 * formal substitutes to its (variable, §13.5.2) actual; a default fills
 * an omitted trailing actual (§13.5.3); a return jumps to the body's end
 * (§13.3). Cut-spanning locals hoist to module registers — static tasks
 * only, per §6.21 — and the shared LoopUnrolling brackets the inlining,
 * before (a call in a constant user loop clones per site) and after (a
 * task-body bound folds once its formal substitutes). Unmarked processes
 * keep their calls untouched, and a task definition with no remaining
 * call site is dropped, to a fixpoint.
 *
 * The induced copy-in and copy-out commits are emitted wrapped in the
 * internal pragmas `(* veriparse_fsm_capture *)` and
 * `(* veriparse_fsm_copyout *)`: the marking rides the AST, so any later
 * cloning keeps the identity, and the FSM lowering adopts and unwraps
 * the markers before its walk — they never reach the output. Running
 * the pass twice is a no-op: the calls are gone after the first run.
 */
class FsmTaskInliner : public TransformationBase
{
public:
    FsmTaskInliner() = default;
    virtual ~FsmTaskInliner() = default;

private:
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /// One module: collect its tasks and declared names, inline every
    /// marked process, then drop the fully-inlined task definitions.
    int inline_module(const AST::Module::Ptr &module);

    /// One marked process: unroll, inline every call, unroll again.
    int inline_process(const AST::Initial::Ptr &initial);

    int inline_calls_in(const AST::Node::Ptr &node, std::set<std::string> &visiting);
    AST::Node::Ptr expand_call(const AST::Call::Ptr &call, std::set<std::string> &visiting);
    int hoist_declaration(const std::string &name, const AST::Node::Ptr &type,
                          const std::string &fn, int ln);

    /// The module under inlining: its tasks, per-task call ordinals
    /// (module-wide, so names stay unique across processes), the
    /// static-hoist registry, which tasks were inlined (disposal), the
    /// declared names for collision checks, and the hoists' insertion
    /// anchor. Reconstructed per module.
    std::map<std::string, AST::Task::Ptr> m_tasks;
    std::map<std::string, unsigned int> m_task_ordinal;
    std::map<std::string, std::string> m_static_hoist;
    std::set<std::string> m_inlined;
    Analysis::UniqueDeclaration::IdentifierSet m_declared;
    std::set<const AST::Node *> m_expanded;
    AST::Module::Ptr m_module;
    AST::Pragmalist::Ptr m_pragmalist;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
