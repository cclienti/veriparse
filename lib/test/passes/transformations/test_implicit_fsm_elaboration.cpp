// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    const auto &module = modules_map[test_name];                                                   \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(module, test_string + "_before.v");                   \
    ASSERT_EQ(0, Passes::Transformations::ImplicitFsmElaboration().run(module));                   \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(module, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");                          \
                                                                                                   \
    std::string test_ref_suffix = "refs/implicit_fsm_";                                            \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr module_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(module_ref != nullptr);                                                            \
                                                                                                   \
    ASSERT_TRUE(module_ref->is_equal(*module, false))

#define TEST_ERROR_SV                                                                              \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    const auto &module = modules_map[test_name];                                                   \
                                                                                                   \
    ASSERT_NE(0, Passes::Transformations::ImplicitFsmElaboration().run(module))

// A straight-line one-shot: three waits become three states plus the hold
// state a one-shot parks in; the preamble is the reset branch with the state
// register going to the first state (ADR-0014 §2, §4, §5.1).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_straight0) { TEST_CORE_SV; }
// Two commits to one register on one path: the first never takes effect
// (ADR-0014 §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_straight_err0) { TEST_ERROR_SV; }
// A register read out of reset that the init segment never assigns
// (ADR-0014 §5.1, §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_straight_err1) { TEST_ERROR_SV; }
// The chip enable: every wait carries the same `iff en`, and the emission
// gates the case and nothing else — the reset stays outside the enable
// (ADR-0014 §5.3). The golden pins that nesting.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_enable0) { TEST_CORE_SV; }
// Some waits qualified, some bare: almost always an oversight, never
// repaired by guessing (ADR-0014 §5.3).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_enable_err0) { TEST_ERROR_SV; }
// Different `iff` conditions: a state-dependent enable is a separate
// feature, not a variant of this one (ADR-0014 §5.3, §15).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_enable_err1) { TEST_ERROR_SV; }
// The enable reads a process register the init segment never assigns: it
// gates every state including the first out of reset, so the register would
// come up undefined — invisible to a 2-state cosim, caught structurally
// (ADR-0014 §5.1, §5.3, §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_enable_err2) { TEST_ERROR_SV; }
// An if/else with cut points in both arms, unequal lengths: the fork spends
// no state of its own, each arm's last state transitions to the same merge
// state, and a cut-point-free branch in the merge segment stays a plain
// conditional inside the action (ADR-0014 §4, §C.3).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch0) { TEST_CORE_SV; }
// An if with no else holding a cut point: the missing else is the
// fall-through path, taking the merge segment in zero extra cycles
// (ADR-0014 §4, §C.4).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch1) { TEST_CORE_SV; }
// A case with a multi-value arm, no default and cut points in the arms:
// one leg per arm, guarded by the disjunction of its matches; the no-match
// path falls through past the endcase under every match negated
// (ADR-0014 §4, §C.3, §C.4).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_case0) { TEST_CORE_SV; }
// Two commits on one runtime path — one in a branch arm, one past the
// merge: the else path commits twice even though the if path is fine
// (ADR-0014 §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch_err0) { TEST_ERROR_SV; }
// A cut point inside a branch before the first wait: the state out of reset
// would be input-dependent — the reset branch cannot fork (ADR-0014 §5.1).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch_err1) { TEST_ERROR_SV; }
// A register written in only one arm and read past the merge: defined-ness
// is the intersection over incoming paths, not the union (ADR-0014 §5.1,
// §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch_err2) { TEST_ERROR_SV; }
// casez in a marked process: wildcard matching is not lowered yet, rejected
// rather than mis-compiled as exact equality (ADR-0014 §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_case_err0) { TEST_ERROR_SV; }
// Two default arms in a forking case: the grammar admits it, IEEE 1800-2017
// §12.5 forbids it in prose, and the guard construction has no condition to
// give the second one — rejected, not crashed on (ADR-0014 §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_case_err1) { TEST_ERROR_SV; }
// A case item with x/z bits in a forking case: item matching is case
// equality (IEEE 1800-2017 §12.5) but the fork guard is built with `==`,
// which such an item can never satisfy — rejected rather than silently
// never taken (ADR-0014 §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_case_err2) { TEST_ERROR_SV; }
// The preamble reads a process register: the entry store out of reset holds
// nothing — even its own '<=' commits only at the edge — so the read is of
// an uninitialised value the reset branch would then replay every reset
// cycle (ADR-0014 §5.1, §6).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_straight_err2) { TEST_ERROR_SV; }
// A branch in the preamble, even without a cut point: the reset branch
// loads constants once, and a conditional there makes the reset value
// input-dependent and re-evaluated on every reset cycle — unlike the source
// initial, which evaluates it exactly once (ADR-0014 §5.1).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_branch_err3) { TEST_ERROR_SV; }
// The §7.3 wait-state idiom: `while (!start) @(posedge clk);` is one state
// holding itself while !start — the back-edge is a self-loop, the exit the
// bare else.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_while0) { TEST_CORE_SV; }
// A rolled repeat with a folded count (§7.2): one state, the shared
// countdown loaded to N-1 by the entering segment, decremented on the lap,
// exited at zero.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat0) { TEST_CORE_SV; }
// repeat (1) rolled: a single pass needs no countdown at all — the body
// runs once, inline (§7.2).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat1) { TEST_CORE_SV; }
// A non-constant repeat count, unmarked: the rolled lowering is forced and
// warned about (§7.2). IEEE §12.7.2 evaluates the count once on entry, so
// the countdown captures it there, sized to the count signal's declared
// width, and a zero count skips the state through the entry guard.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat2) { TEST_CORE_SV; }
// A rolled for (§7.2): the author's index register honours init, test and
// step — the init and step commit once per entry/lap, and their values
// substitute forward within their own segment (§6.1), which is when the
// source evaluates the test and the body's first reads.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_for0) { TEST_CORE_SV; }
// §8: break transitions past the innermost loop, continue takes its
// back-edge — once states are explicit a jump is just an edge.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_jump0) { TEST_CORE_SV; }
// A path through a while body that reaches the head again without crossing
// a cut point: a zero-delay lap (IEEE 1800-2017 §9.2.2.1, ADR-0014 §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_while_err0) { TEST_ERROR_SV; }
// A while with no cut point at all: no static exit, no hardware meaning
// (IEEE 1800-2017 §9.2.2.1, ADR-0014 §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_while_err1) { TEST_ERROR_SV; }
// (* veriparse_no_unroll *) on a loop without a cut point: the hint is
// inert — the loop runs in zero time, there is no state to save (§7.2).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_loop_err0) { TEST_ERROR_SV; }
// break outside any loop the CFG sees: nothing to jump within (§8).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_jump_err0) { TEST_ERROR_SV; }
// A rolled for whose step assigns a different register than its init: the
// construct's contract names one index (§7.2).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_for_err0) { TEST_ERROR_SV; }
// Nested rolled repeats: both would drive the one shared countdown (§15),
// the inner reload clobbering the outer's remaining count — rejected, not
// silently mis-counted (§7.2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat_err0) { TEST_ERROR_SV; }
// repeat (1) rolled with a jump inside: the single pass still owns its
// break/continue — break falls through past the pass, never out of an
// enclosing loop (IEEE 1800-2017 §12.7.2, ADR-0014 §8).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat3) { TEST_CORE_SV; }
// A keyword-width count (`int n`): no packed dims, 32 bits all the same —
// the countdown takes the full declared width (§7.2).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat4) { TEST_CORE_SV; }
// A constant repeat count that folds negative: the loop cannot execute a
// negative number of times, and tools disagree on what it means — almost
// always a parameterization off-by-N, rejected loudly (§7.2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat_err1) { TEST_ERROR_SV; }
// Two sequential rolled fors sharing one module-level index: the first
// loop's exit step and the second's entry init land in one segment, and the
// induced commits coalesce blocking-style — last wins — instead of tripping
// §6 (§7.2, §7.3's one-declared-counter idiom).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_for1) { TEST_CORE_SV; }
// The rolled for's index resolves to an input port: the machine cannot
// drive it (§7.2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_for_err1) { TEST_ERROR_SV; }
// A non-constant repeat count that is not a plain signal: no declared
// width for the countdown to take (§7.2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_repeat_err2) { TEST_ERROR_SV; }
// A rolled for whose index is declared nowhere at module level (§7.2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_for_err2) { TEST_ERROR_SV; }
// The perpetual form (§2): a multi-cycle prologue above the loop, then
// forever on the §7.3 back-edge with no exit test — and no hold state,
// since no path ends the process.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_forever0) { TEST_CORE_SV; }
// forever escaped by break (§8): the jump is the only way past the loop,
// and the path it opens ends the process — the hold state returns.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_forever1) { TEST_CORE_SV; }
// forever with no cut point: the §9 zero-delay row, on the perpetual form.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_forever_err0) { TEST_ERROR_SV; }
// The A.1 WAIT_SEND idiom with a compound condition: the empty leg that
// takes `if (!(go && rdy))` yet skips its identical while carries the
// contradiction through a negated conjunction — flattened parts against
// the whole — and prunes like the single-identifier form (§C.4).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_prune0) { TEST_CORE_SV; }
// Two rolled repeats over the same count in one zero-time segment: the
// walk's own complement pair — `n == 0` from the first skip, `n != 0` from
// the second entry — names an empty path, pruned rather than emitted as a
// dead arm (§C.4).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_prune1) { TEST_CORE_SV; }
// An impure system call in a fork condition: the walk assumes a condition
// reads stably within its zero-time segment — pruning and guard reuse are
// both wrong otherwise — so it is rejected, not silently mis-pruned
// (ADR-0014 §9, §C.4).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_prune_err0) { TEST_ERROR_SV; }
// while (1'b1) is the perpetual form spelled differently: the constant
// test folds at the loop head, no dead exit leg, no hold state beyond the
// one break opens — the same machine forever produces (§2, §7.3).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_forever2) { TEST_CORE_SV; }

// —— The §9 table, row by row (§12 phase 7) ——

// Cut points over different signals: multi-clock scheduling is out of
// scope, and picking one would mis-compile (§2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_clock_err0) { TEST_ERROR_SV; }
// A level sensitivity `@(sig)`: no clock at all (§2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_clock_err1) { TEST_ERROR_SV; }
// '#' delay: simulation timing with no hardware meaning (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_delay_err0) { TEST_ERROR_SV; }
// A system task: no hardware meaning — the mark landed on testbench
// code? (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_systask_err0) { TEST_ERROR_SV; }
// A level-sensitive wait: not an edge, no boundary to cut at (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_wait_err0) { TEST_ERROR_SV; }
// A free blocking assignment: '=' names a combinational value the
// lowering does not handle yet (§6, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_blocking_err0) { TEST_ERROR_SV; }
// A marked always whose body holds a wait: not compiled — the message
// carries the one-line initial-forever rewrite (§9, §15.1).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_always_err0) { TEST_ERROR_SV; }
// The mark on an item that is not a process (§2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_mark_err0) { TEST_ERROR_SV; }
// The mark on an initial with no wait: nothing to compile (§2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_mark_err1) { TEST_ERROR_SV; }
// Reset neither hinted nor uniquely inferable: two candidate inputs (§5,
// §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_reset_err0) { TEST_ERROR_SV; }
// fork/join: concurrent control flow the state model cannot express (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_fork_err0) { TEST_ERROR_SV; }
// disable: abortive control flow the state model cannot express (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_disable_err0) { TEST_ERROR_SV; }
// A task call: a cut point inside it would be invisible — v1 does not
// inline to find out (§9, §15).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_task_err0) { TEST_ERROR_SV; }
// A system function outside the constant/query subset in expression
// position (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_sysfunc_err0) { TEST_ERROR_SV; }
// A function that writes non-local state, called in the process:
// expression position is no place for a side effect (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_func_err0) { TEST_ERROR_SV; }
// A pure function passes through as the ordinary combinational call it
// is (§9): the accepted half of the same row.
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_func0) { TEST_CORE_SV; }
// A target the marked process shares with another process: the output
// would not conform to IEEE §9.2.2.4, which is the stronger reason to
// refuse (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_multidrive_err0) { TEST_ERROR_SV; }
// Mixed posedge/negedge over one signal: one clock, one edge, per
// process (§2, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_clock_err2) { TEST_ERROR_SV; }
// The §20.9 bit-vector functions are as synthesizable and stable as the
// §20.5-§20.8 queries: $countones passes through (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_sysfunc0) { TEST_CORE_SV; }
// $random in a cut-point-free condition: no fork to poison, but the call
// would land verbatim in the always_ff — rejected wherever it appears
// (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_sysfunc_err1) { TEST_ERROR_SV; }
// $random in an `iff` wait condition: the enable is read at every state's
// entry, the least stable place of all (§5.3, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_sysfunc_err2) { TEST_ERROR_SV; }
// The impurity hides inside the called function's body: the purity check
// scans what the function calls, not only what it assigns (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_func_err1) { TEST_ERROR_SV; }
// A static-lifetime function writing a local: the local carries state
// across calls — successive evaluations differ, which breaks the walk's
// stable-read assumption; `automatic` is the one-word fix (§9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_func_err2) { TEST_ERROR_SV; }
// The other writer hides behind a task call: the task's body drives the
// register, the calling process makes it a driver (IEEE §9.2.2.4, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_multidrive_err1) { TEST_ERROR_SV; }
// The other writer sits inside a generate block (IEEE §9.2.2.4, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_multidrive_err2) { TEST_ERROR_SV; }
// A block-local variable shadowing the FSM's register name in another
// process: scope-blind name matching must not refuse the legal design
// (IEEE §9.2.2.4, §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_multidrive0) { TEST_CORE_SV; }
// Another process writes through an indexed part-select whose index is
// the FSM's register: the index is a read, not a drive (IEEE §9.2.2.4,
// §9).
TEST(PassesTransformation_ImplicitFsmElaboration, fsm_multidrive1) { TEST_CORE_SV; }
