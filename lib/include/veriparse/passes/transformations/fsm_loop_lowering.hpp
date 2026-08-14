// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_FSM_LOOP_LOWERING
#define VERIPARSE_PASSES_TRANSFORMATIONS_FSM_LOOP_LOWERING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Canonicalize the rolled bounded loops of the (* veriparse_fsm *)
 * processes (ADR-0014 §7.2) — after FsmTaskInliner, before the walk.
 *
 * A rolled `repeat (count)` becomes a countdown while loop: the count
 * loads into the depth's shared register (captured at entry, IEEE
 * §12.7.3), the head tests `cnt != 0`, and the lap decrements first — so
 * a `continue` skips nothing it should not. A folded count of 0 deletes
 * the loop, a count of 1 becomes a run-once `forever` whose `continue`s
 * and tail turn into `break`. A rolled `for` keeps the author's index
 * register: the init precedes the loop, the step closes the body — and
 * re-arms every `continue` — while the head keeps the author's test.
 * `while` and `forever` pass through: after this pass, they are the only
 * loop forms the walk meets.
 *
 * The induced loads, decrements and steps are emitted wrapped in the
 * internal `(* veriparse_fsm_capture *)` marker, so the walk gives them
 * the §6.1 induced treatment — forward substitution within their
 * segment, coalescing of superseded commits — and the shared countdown
 * registers (`<prefix>_cnt`, `_cnt2`, ... one per repeat-nesting depth)
 * are declared at module level by this pass. A bounded loop that
 * survived unrolling without `(* veriparse_no_unroll *)` keeps the §8
 * refusal: rolled is opt-in.
 */
class FsmLoopLowering : public TransformationBase
{
public:
    FsmLoopLowering() = default;
    virtual ~FsmLoopLowering() = default;

private:
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    int lower_module(const AST::Module::Ptr &module);
    int lower_process(const AST::Initial::Ptr &initial);
    int lower_list(const AST::Node::ListPtr &stmts);
    int lower_slot(AST::Node::Ptr &slot);
    int lower_repeat(const AST::RepeatStatement::Ptr &loop, bool kept_rolled,
                     AST::Node::ListPtr &out);
    int lower_for(const AST::ForStatement::Ptr &loop, bool kept_rolled, AST::Node::ListPtr &out);
    int check_rolled(const AST::Node::Ptr &loop, bool kept_rolled) const;

    /// Depth d's countdown register name for the current process.
    std::string cnt_name(unsigned int depth) const;

    /// The current module, its declared names, the per-process prefix,
    /// the repeat-nesting depth during the rewrite, and each depth's
    /// countdown width — declared at module level once the process is
    /// lowered. Reconstructed per module / per process.
    AST::Module::Ptr m_module;
    AST::Pragmalist::Ptr m_pragmalist;
    Analysis::UniqueDeclaration::IdentifierSet m_declared;
    std::string m_prefix;
    unsigned int m_depth = 0;
    std::vector<unsigned int> m_cnt_widths;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
