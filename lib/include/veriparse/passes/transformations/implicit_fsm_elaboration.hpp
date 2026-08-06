// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION

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
 * @brief Compile a process marked (* veriparse_fsm *) into an explicit FSM
 * (ADR-0014).
 *
 * The process body is cut at every EventStatement: the statements before the
 * first wait become the reset branch (§5.1), each segment between waits
 * becomes one state of a generated `always_ff`, and a one-shot process parks
 * in an appended hold state (§2). The reset signal is taken from the
 * `veriparse_reset` hint or inferred from the module inputs (§5).
 *
 * Straight-line subset: segments hold nonblocking assignments to plain
 * registers. Branches, loops, blocking temporaries and the chip enable are
 * rejected with a diagnostic rather than mis-lowered (§9).
 */
class ImplicitFsmElaboration : public TransformationBase
{
public:
    ImplicitFsmElaboration() = default;

private:
    /**
     * @return zero on success
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /// One state's worth of statements: the run between two cut points.
    struct Segment
    {
        AST::Node::ListPtr statements;
    };

    int compile_process(const AST::Module::Ptr &module, const AST::Node::Ptr &parent,
                        const AST::Pragmalist::Ptr &pragmalist, const AST::Initial::Ptr &initial,
                        const std::string &prefix);

    /// Flatten the process body into atoms (assignments and waits), walking
    /// blocks transparently and rejecting what the lowering cannot express.
    int flatten_body(const AST::Node::Ptr &node, AST::Node::ListPtr atoms);

    /// Check one wait: exactly one Sens, posedge/negedge, no `iff`, and the
    /// same edge over the same signal as every other wait of the process.
    int check_wait(const AST::EventStatement::Ptr &event, AST::Sens::Ptr &clock);

    /// §5: `veriparse_reset` hint, else the unique matching module input.
    /// Fills the signal name and its active level.
    int find_reset(const AST::Module::Ptr &module, const AST::Pragmalist::Ptr &pragmalist,
                   std::string &reset_name, bool &active_low);

    /// §6 on the straight-line subset: one commit per register per segment,
    /// and no register read before a path out of reset has written it.
    int check_segments(const AST::Node::ListPtr &init_stmts, const std::vector<Segment> &segments);

    AST::Node::ListPtr emit(const AST::Module::Ptr &module, const AST::Sens::Ptr &clock,
                            const std::string &reset_name, bool active_low,
                            const AST::Node::ListPtr &init_stmts,
                            const std::vector<Segment> &segments, const std::string &prefix);
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
