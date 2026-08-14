// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/fsm_loop_lowering.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

// The pass on its own: the golden is the canonicalized process — every
// rolled bounded loop a while (or run-once forever) whose induced loads,
// decrements and steps ride the (* veriparse_fsm_capture *) marker, the
// countdown registers declared at module level.
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
    ASSERT_EQ(0, Passes::Transformations::FsmLoopLowering().run(module));                          \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(module, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");                          \
                                                                                                   \
    std::string test_ref_suffix = "refs/fsm_lowered_";                                             \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr module_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(module_ref != nullptr);                                                            \
                                                                                                   \
    ASSERT_TRUE(module_ref->is_equal(*module, false))

// A dynamic rolled repeat: the count loads captured at entry, the head
// tests the countdown, the lap decrements first (§7.2, §12.7.3).
TEST(PassesTransformation_FsmLoopLowering, fsm_repeat1) { TEST_CORE_SV; }
// Nested rolled repeats: each nesting depth owns its countdown (cnt2).
TEST(PassesTransformation_FsmLoopLowering, fsm_repeat4) { TEST_CORE_SV; }
// A rolled for: the author's index register, init before the loop, the
// step closing the body and re-arming every continue (§12.7.1).
TEST(PassesTransformation_FsmLoopLowering, fsm_for0) { TEST_CORE_SV; }
// break and continue through a rolled loop survive the rewrite (§8).
TEST(PassesTransformation_FsmLoopLowering, fsm_jump2) { TEST_CORE_SV; }
