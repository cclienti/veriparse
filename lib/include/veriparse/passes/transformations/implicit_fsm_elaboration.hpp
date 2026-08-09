// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <map>
#include <set>
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
 * Actions hold nonblocking assignments to plain registers and the branches
 * that fork or ride along the path cover (§4, §C.3): an if or case whose
 * arms hold cut points forks the state graph — arms of unequal length meet
 * at the same merge state — while a cut-point-free branch stays a plain
 * conditional inside one state's action. Loops and blocking temporaries are
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

    /// One path of the path cover (§C.3): the guard it is taken under —
    /// over entry values, null meaning unconditional — the statements it
    /// executes, and the state it enters. Cut-point-free branches stay
    /// verbatim inside the action (§4), so the path count follows the
    /// reachable cut points, never the conditionals between them.
    struct Transition
    {
        AST::Node::Ptr guard;
        AST::Node::ListPtr action;
        std::size_t next;
    };

    /// One state per cut point, in source order; its transitions in
    /// enumeration order, first match wins. The hold state (§2) is the
    /// index one past the last.
    struct State
    {
        AST::EventStatement::Ptr wait;
        std::vector<Transition> out;
        bool walked = false;
    };

    /// A position in the statement tree during path enumeration: the
    /// innermost list is walked first, exhausted frames pop back to the
    /// enclosing continuation — which is how the merge after a branch is
    /// reached from every arm.
    struct Frame
    {
        AST::Node::ListPtr stmts;
        AST::Node::List::iterator it;
    };

    int compile_process(const AST::Module::Ptr &module, const AST::Node::Ptr &parent,
                        const AST::Pragmalist::Ptr &pragmalist, const AST::Initial::Ptr &initial,
                        const std::string &prefix);

    /// Collect the process's waits in source order, validating each
    /// statement against what the lowering can express. Fills @p has_wait
    /// with whether the subtree holds a cut point, recording forking
    /// branches and wait indices so the path walk resolves both in O(1).
    int collect_body(const AST::Node::Ptr &node, std::vector<AST::EventStatement::Ptr> &waits,
                     AST::Sens::Ptr &clock, bool &has_wait);

    /// Push a statement as an enumeration frame: a block contributes its
    /// list, a single statement a one-element list, null nothing.
    static void push_frame(std::vector<Frame> &frames, const AST::Node::Ptr &node);

    /// Enumerate the path cover (§C.4 step 3) from the position in
    /// @p frames: fork at branches whose arms hold a cut point, copy
    /// cut-point-free statements into the running action, end each path at
    /// the next wait — or at the hold state when the process ends.
    int walk_paths(std::size_t from, const AST::Node::Ptr &guard, AST::Node::ListPtr action,
                   std::vector<Frame> frames, std::vector<State> &states,
                   std::vector<Transition> &entry);

    /// Check one wait: exactly one Sens, posedge/negedge, and the same edge
    /// over the same signal as every other wait of the process.
    int check_wait(const AST::EventStatement::Ptr &event, AST::Sens::Ptr &clock);

    /// §2 extended to the chip enable (§5.3): every wait bare, or every wait
    /// carrying the same `iff` condition — structurally equal after the
    /// passes that ran before this one. Fills the uniform enable, null when
    /// the waits are bare.
    int check_enable(const std::vector<AST::EventStatement::Ptr> &waits, AST::Node::Ptr &enable);

    /// §5: `veriparse_reset` hint, else the unique matching module input.
    /// Fills the signal name and its active level.
    int find_reset(const AST::Module::Ptr &module, const AST::Pragmalist::Ptr &pragmalist,
                   std::string &reset_name, bool &active_low);

    /// §6 over the path cover: at most one commit per register on any
    /// runtime path through an action, and no register read before every
    /// path out of reset has written it — the enable included, since it is
    /// read at every state's entry (§5.3). Must-defined sets propagate over
    /// the acyclic state graph in source order.
    int check_paths(const AST::Node::ListPtr &init_stmts, const std::vector<State> &states,
                    std::size_t entry_next, const AST::Node::Ptr &enable);

    AST::Node::ListPtr emit(const AST::Module::Ptr &module, const AST::Sens::Ptr &clock,
                            const AST::Node::Ptr &enable, const std::string &reset_name,
                            bool active_low, const AST::Node::ListPtr &init_stmts,
                            const std::vector<State> &states, std::size_t entry_next,
                            const std::string &prefix);

    /// Branches whose subtree holds a cut point — the ones that fork the
    /// path walk — and each wait's state index, both filled per process by
    /// collect_body.
    std::set<const AST::Node *> m_forking;
    std::map<const AST::EventStatement *, std::size_t> m_wait_index;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
