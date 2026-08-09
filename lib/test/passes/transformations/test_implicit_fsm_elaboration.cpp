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
