// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_IMPLICIT_FSM_ELABORATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
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
 * arms hold cut points — or a jump — forks the state graph, arms of unequal
 * length meeting at the same merge state, while a cut-point-free branch
 * stays a plain conditional inside one state's action. Loops the CFG keeps
 * become back-edges (§7.3), the rolled bounded forms carrying their induced
 * storage (§7.2): the shared countdown for a repeat, the author's index
 * register for a for, with the blocking init/step substituted forward
 * within their own segment (§6.1). break and continue are edges over those
 * loops (§8). The perpetual `forever` form and free blocking temporaries
 * are rejected with a diagnostic rather than mis-lowered (§9).
 */
class ImplicitFsmElaboration : public TransformationBase
{
public:
    /// §10.2: what the source does not state and the RTL should not have
    /// to be reverse-engineered for — the encoding, the naming, the reset
    /// contract and the transition structure, per compiled process. The
    /// JSON state map and the graphviz view are both printed from this.
    struct FsmReport
    {
        struct State
        {
            std::string name;
            unsigned long value = 0;
            int line = 0;
        };
        struct Transition
        {
            std::string from;
            std::string to;
            std::string guard;  ///< empty = unconditional
            std::string action; ///< the register updates, `;`-joined
        };
        /// §6.2: a decoded output and its per-state value, rendered.
        struct Decode
        {
            std::string signal;
            std::vector<std::pair<std::string, std::string>> values;
        };
        struct Process
        {
            std::string module_name;
            std::string state_variable;
            unsigned int width = 0;
            std::string encoding;
            std::string entry; ///< the state reset transitions into
            bool has_hold = false;
            std::string reset_signal;
            int reset_active_level = 1;
            std::string reset_kind;
            std::vector<std::string> reset_registers;
            std::vector<State> states;
            std::vector<Transition> transitions;
            std::vector<Decode> decodes;
        };
        std::vector<Process> processes;
    };

    ImplicitFsmElaboration() = default;
    explicit ImplicitFsmElaboration(FsmReport *report,
                                    const Analysis::Module::ModulesMap *modules = nullptr)
        : m_report(report), m_modules(modules)
    {
    }

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
        /// §6.2: the decoded outputs' values at this path's end — the
        /// arrival values of the state it enters.
        std::map<std::string, AST::Node::Ptr> decode;
    };

    /// One state per cut point, in source order; its transitions in
    /// enumeration order, first match wins. The hold state (§2) is the
    /// index one past the last. The stem is the §10.1 label composition
    /// naming the state — empty falls back to the ordinal.
    struct State
    {
        AST::EventStatement::Ptr wait;
        std::vector<Transition> out;
        bool walked = false;
        std::string stem;
    };

    /// A position in the statement tree during path enumeration: the
    /// innermost list is walked first, exhausted frames pop back to the
    /// enclosing continuation — which is how the merge after a branch is
    /// reached from every arm. A frame carrying @c loop is the body of a
    /// loop the CFG keeps: reaching its end takes the back-edge instead of
    /// popping through (§7.2, §7.3). A frame carrying @c label is a named
    /// block (§10.1): the labels in force compose outward-in to name the
    /// states cut inside them.
    struct Frame
    {
        AST::Node::ListPtr stmts;
        AST::Node::List::iterator it;
        const AST::Node *loop = nullptr;
        std::string label;
        /// §6 temporaries this frame's block declares: their environment
        /// entries die when the frame pops — the scope is the lifetime,
        /// and the declaration supplies the wire's type.
        std::vector<AST::Var::Ptr> decls;
    };

    /// §6 scoping context during collection: which temporaries are
    /// readable here, and which may be written — writes reset at branch
    /// arms, so a conditional value cannot leak into an outer name.
    struct TempScope
    {
        std::set<std::string> visible;
        std::set<std::string> writable;
        bool verbatim = false; ///< inside a cut-point-free branch kept verbatim
    };

    /// A loop the CFG keeps — §7.3 data-dependent, or §7.2 rolled with its
    /// induced storage contract.
    struct LoopInfo
    {
        enum class Kind
        {
            WHILE,
            REPEAT,
            FOR,
            FOREVER
        };
        Kind kind = Kind::WHILE;
        AST::Node::Ptr cond; ///< while/for: the test; repeat: the count; forever: null
        AST::Node::Ptr body;
        std::string index;        ///< rolled for: the author's index register
        AST::Node::Ptr init_rhs;  ///< rolled for: the entry value
        AST::Node::Ptr step_rhs;  ///< rolled for: the per-lap value
        bool count_known = false; ///< rolled repeat: the count folded
        unsigned long count_value = 0;
        unsigned int depth = 0; ///< rolled repeat: nesting depth, one countdown per depth
    };

    /// §6.1 for the induced registers: the blocking init/step values,
    /// substituted forward within their own segment.
    using Env = std::map<std::string, AST::Node::Ptr>;

    int compile_process(const AST::Module::Ptr &module, const AST::Node::Ptr &parent,
                        const AST::Pragmalist::Ptr &pragmalist, const AST::Initial::Ptr &initial,
                        const std::string &prefix);

    /// §6.2: build the per-state decode arms from the arrivals — totality
    /// was checked at record time; this checks stability and coherency and
    /// fills m_decode_arms / m_init_decode.
    int build_decode(const std::vector<State> &states, const std::vector<Transition> &entry,
                     std::size_t entry_next);

    /// §6.2: the emitted always_comb over the state register.
    AST::Node::Ptr emit_decode(const std::string &state_reg,
                               const std::vector<std::string> &state_names,
                               const std::string &reset_name, bool active_low,
                               const std::string &fn, int ln) const;

    /// Collect the process's waits in source order, validating each
    /// statement against what the lowering can express. Fills @p has_wait
    /// with whether the subtree holds a cut point, recording forking
    /// branches, wait indices and §6 temporaries so the path walk resolves
    /// them all in O(1). @p scope carries the temporaries in force, by
    /// value: a block's declarations reach its later siblings, never its
    /// parent's.
    int collect_body(const AST::Node::Ptr &node, std::vector<AST::EventStatement::Ptr> &waits,
                     AST::Sens::Ptr &clock, bool &has_wait, TempScope scope);

    /// §6.1: one blocking assignment — the value taken over entry values,
    /// a constant folded and truncated to the declared width inline,
    /// anything else materialized as a wire typed by @p decl.
    int process_blocking(const AST::Var::Ptr &decl, const std::string &target,
                         const AST::Node::Ptr &rhs, Env &env, const std::string &fn, int ln);

    /// The §6.1 wire for one value: typed by the temporary's declaration,
    /// shared only when expression AND declared type both match.
    std::string materialize_temp(const AST::Var::Ptr &decl, const std::string &temp,
                                 const AST::Node::Ptr &value);

    /// The innermost frame declaring @p name — the walk's scope lookup.
    static AST::Var::Ptr find_temp_decl(const std::vector<Frame> &frames, const std::string &name);

    /// §6.1: a substituted expression must not still read a temporary the
    /// environment no longer carries — dead scope, or not yet assigned.
    int check_temp_reads(const AST::Node::Ptr &node, const Env &env);

    /// Validate and register a loop the CFG keeps (§7.2 rolled, §7.3
    /// data-dependent); @p kept_rolled when `(* veriparse_no_unroll *)`
    /// asked for it, false when the loop simply survived the unroller.
    int collect_loop(const AST::Node::Ptr &node, bool kept_rolled,
                     std::vector<AST::EventStatement::Ptr> &waits, AST::Sens::Ptr &clock,
                     bool &has_wait);

    /// Push a statement as an enumeration frame: a block or pragma list
    /// contributes its list, a single statement a one-element list, null
    /// nothing. @p loop marks the frame as a loop body; a named block
    /// carries its §10.1 label.
    static void push_frame(std::vector<Frame> &frames, const AST::Node::Ptr &node,
                           const AST::Node *loop = nullptr);

    /// The §10.1 label composition in force at a walk position.
    static std::string labels_of(const std::vector<Frame> &frames);

    /// Enumerate the path cover (§C.4 step 3) from the position in
    /// @p frames: fork at branches whose arms hold a cut point, copy
    /// cut-point-free statements into the running action, end each path at
    /// the next wait — or at the hold state when the process ends. @p env
    /// carries induced-register values substituted within the segment;
    /// @p lapped the loops entered since the last cut point, to reject a
    /// zero-delay lap.
    int walk_paths(std::size_t from, const AST::Node::Ptr &guard, AST::Node::ListPtr action,
                   std::vector<Frame> frames, Env env, std::set<const AST::Node *> lapped,
                   std::vector<State> &states, std::vector<Transition> &entry);

    /// Append an induced-register commit to an action, coalescing away a
    /// prior induced commit to the same register (§7.2): blocking
    /// last-wins — the abandoned value was the pass's own, a dead
    /// decrement past a break or a superseded init, and its reads were
    /// carried by the environment. Author commits are never touched and §6
    /// still flags them.
    void push_induced(const AST::Node::ListPtr &action, const std::string &target,
                      const AST::Node::Ptr &rhs, const std::string &fn, int ln);

    /// Fork at a loop head — on entry or on the back-edge: the enter leg
    /// pushes the body frame under the loop's guard, the exit leg continues
    /// past it. Rolled forms commit their induced register into the action
    /// (§7.2): the countdown load or decrement, the index init or step.
    int loop_fork(const AST::Node *loop, bool entering, std::size_t from,
                  const AST::Node::Ptr &guard, const AST::Node::ListPtr &action,
                  const std::vector<Frame> &frames, const Env &env,
                  const std::set<const AST::Node *> &lapped, std::vector<State> &states,
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

    /// The loops the CFG keeps, filled per process by collect_body.
    std::map<const AST::Node *, LoopInfo> m_loops;

    /// The shared countdowns (§15: one per repeat-nesting depth,
    /// re-initialised on entry): sequential repeats at one depth share a
    /// register, nested ones each own their depth's, so an inner reload
    /// leaves the outer count alone. m_cnt_widths[d] is depth d's width —
    /// zero when nothing at that depth needs one.
    std::string m_cnt_name;
    std::vector<unsigned int> m_cnt_widths;
    unsigned int m_repeat_depth = 0;

    /// Depth d's countdown register name: the bare prefix at depth zero,
    /// then cnt2, cnt3, ...
    std::string cnt_name(unsigned int depth) const
    {
        return depth == 0 ? m_cnt_name : m_cnt_name + std::to_string(depth + 1);
    }

    /// The induced-register commits this pass created (§7.2): when a later
    /// induced commit to the same register lands in the same action —
    /// sequential rolled fors sharing one index — the earlier one coalesces
    /// away, blocking-style, instead of tripping §6. Author commits never
    /// do.
    std::set<const AST::Node *> m_induced;

    /// Whether some path ends the process — set by the walk when it records
    /// a transition to the hold index. A perpetual machine has none, and
    /// then no hold state is emitted (§2).
    bool m_hold_needed = false;

    /// §3 hints steering the emission: the state encoding, and the reset
    /// flavour of the generated always_ff.
    enum class Encoding
    {
        BINARY,
        ONE_HOT,
        GRAY
    };
    Encoding m_encoding = Encoding::BINARY;
    bool m_async_reset = false;

    /// §6 temporaries of the walked segments (name → declaration, for the
    /// materialized wire's type), and the §6.1 wires the emission owes:
    /// name, the substituted value, and the temporary supplying the type.
    std::map<std::string, AST::Var::Ptr> m_temps;
    /// §6.2 decoded outputs: name -> the module-level declaration (anchor).
    std::map<std::string, AST::Node::Ptr> m_decoded;
    /// Targets taking '<=' anywhere in the process: the §6.2 discipline
    /// check is order-independent through this set.
    std::set<std::string> m_nba_targets;
    /// §6.2: the init segment's decode values (comb reset + default arm).
    std::map<std::string, AST::Node::Ptr> m_init_decode;
    /// §6.2 per-state arms: name -> (guard, value) chain; a single entry
    /// with a null guard is unconditional, else the last entry is the else.
    std::vector<std::map<std::string, std::vector<std::pair<AST::Node::Ptr, AST::Node::Ptr>>>>
        m_decode_arms;
    /// The module under compilation, for by-name declaration lookups
    /// during collection.
    AST::Module::Ptr m_walk_module;
    /// The marked process under compilation, excluded from the §6.2
    /// foreign-driver scan.
    AST::Pragmalist::Ptr m_walk_pragmalist;
    struct MaterializedWire
    {
        std::string name;
        AST::Node::Ptr value;
        AST::Var::Ptr temp;
    };
    std::vector<MaterializedWire> m_wires;

    /// The per-process declaration prefix, for the wire names.
    std::string m_prefix;

    /// §10.2 collection point, null when nobody asked.
    FsmReport *m_report = nullptr;

    /// The parsed modules, when the driver has them: how an instance's
    /// output-port connections become visible to the §9.2.2.4 multi-driver
    /// check. Null — or an instantiated module missing from the map, a
    /// black box — skips that instance.
    const Analysis::Module::ModulesMap *m_modules = nullptr;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
