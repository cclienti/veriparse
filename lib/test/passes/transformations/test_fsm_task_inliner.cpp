// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/fsm_task_inliner.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

// The pass on its own: the golden is the inlined-but-not-lowered process
// — the labelled call-site blocks, the hoisted registers, and the
// induced commits still wrapped in their (* veriparse_fsm_capture *) /
// (* veriparse_fsm_copyout *) markers, the pass's output contract.
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
    ASSERT_EQ(0, Passes::Transformations::FsmTaskInliner().run(module));                           \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(module, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");                          \
                                                                                                   \
    /* A second run is a no-op: the calls are gone. */                                             \
    ASSERT_EQ(0, Passes::Transformations::FsmTaskInliner().run(module));                           \
    test_helpers.render_node_to_yaml_file(module, test_string + "_rerun.yaml");                    \
                                                                                                   \
    std::string test_ref_suffix = "refs/fsm_inline_";                                              \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr module_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(module_ref != nullptr);                                                            \
                                                                                                   \
    ASSERT_TRUE(module_ref->is_equal(*module, false))

// The one-task sequencer with the rolled countdown: the capture markers
// ride the copy-ins through the post-substitution unroll (§7.4).
TEST(PassesTransformation_FsmTaskInliner, fsm_task1) { TEST_CORE_SV; }
// Nested calls: the inner expansion's copy-out into the outer formal
// arrives marked, and both fully-inlined definitions are dropped.
TEST(PassesTransformation_FsmTaskInliner, fsm_task_nest0) { TEST_CORE_SV; }
// Defaults fill omitted actuals before expansion (§13.5.3).
TEST(PassesTransformation_FsmTaskInliner, fsm_dflt0) { TEST_CORE_SV; }
// Early return restructures to the block's end before splicing (§13.3).
TEST(PassesTransformation_FsmTaskInliner, fsm_ret0) { TEST_CORE_SV; }
// Interface members as actuals, at the pass level: the copy-out keeps the
// whole path rather than the member's leaf name (IEEE 1800-2017 §25.3).
TEST(PassesTransformation_FsmTaskInliner, fsm_iface1) { TEST_CORE_SV; }
// A ref formal aliasing an interface member: a member is a variable, so the
// §13.5.2 net refusal cannot apply to it.
TEST(PassesTransformation_FsmTaskInliner, fsm_iface2) { TEST_CORE_SV; }
// The copy-out of a branch-local call keeps the member's whole path.
TEST(PassesTransformation_FsmTaskInliner, fsm_iface3) { TEST_CORE_SV; }
