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
