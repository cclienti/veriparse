// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/loop_unrolling.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE_BODY(SVMODE)                                                                     \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(SVMODE);                                                                   \
    verilog.parse(test_helpers.get_verilog_filename(test_name));                                   \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* Get all modules in the file */                                                              \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    const auto &module = modules_map[test_name];                                                   \
                                                                                                   \
    /* Get all functions in the file */                                                            \
    Passes::Analysis::Module::FunctionMap function_map;                                            \
    Passes::Analysis::Module::get_function_dictionary(module, function_map);                       \
                                                                                                   \
    /* apply the transformation */                                                                 \
    test_helpers.render_node_to_verilog_file(module, test_string + "_before.v");                   \
    Passes::Analysis::UniqueDeclaration::seed(0);                                                  \
    Passes::Transformations::LoopUnrolling(function_map).run(module);                              \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(module, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(module, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/loop_unrolling_";                                          \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr module_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(module_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(module_ref->is_equal(*module, false))

#define TEST_CORE TEST_CORE_BODY(false)
#define TEST_CORE_SV TEST_CORE_BODY(true)

TEST(PassesTransformation_LoopUnrolling, loop_unrolling0) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, loop_unrolling1) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, loop_unrolling2) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, loop_unrolling3) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, repeat0) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, repeat1) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, repeat3) { TEST_CORE; }
// A for loop nested inside a repeat: the inner unroll replaces the iteration
// block's statement list, and the repeat must splice the replaced list — not a
// stale pre-recursion pointer that still holds the un-unrolled loop.
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_repeat_nested0) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, scope0) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, scope1) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, scope2) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, defparam1) { TEST_CORE; }
TEST(PassesTransformation_LoopUnrolling, defparam2) { TEST_CORE; }
// `break` (§12.8) lowered structurally: each iteration's remainder and all later
// iterations nest inside `if (!cond)`, so it stops at the first taken break
// (ADR-0005 §3.2).
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_break0) { TEST_CORE_SV; }
// `continue` lowered structurally: each iteration independently guards its own
// remainder with `if (!cond)` (later iterations are unaffected).
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_continue0) { TEST_CORE_SV; }
// Same lowering in an `initial` block — the context that exposed the X-propagation
// question: the remainder sits in the `else` of the jump condition, so it runs when
// the condition is not true (including X), matching the source.
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_break_init0) { TEST_CORE_SV; }
// The same break/continue lowering applies to `repeat` loops (§12.8).
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_repeat_break0) { TEST_CORE_SV; }
TEST(PassesTransformation_LoopUnrolling, loop_unrolling_repeat_continue0) { TEST_CORE_SV; }
