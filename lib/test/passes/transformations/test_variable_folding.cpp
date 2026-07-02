// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/variable_folding.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE                                                                                  \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.parse(test_helpers.get_verilog_filename(test_name));                                   \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");                     \
    /* apply the transformation */                                                                 \
    Passes::Transformations::VariableFolding().run(source);                                        \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/variable_folding_";                                        \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.sv");                  \
    Passes::Transformations::VariableFolding().run(source);                                        \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".sv");                         \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/variable_folding_";                                        \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

TEST(PassesTransformation_VariableFolding, unrolled0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, ifstmt0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, ifstmt1) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, ifstmt2) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, repeat0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, repeat1) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, repeat2) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, while0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, while1) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, while2) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, for0) { TEST_CORE; }
// A loop whose bound is non-constant is left intact; a later read of a
// loop-assigned variable must NOT fold to a stale partial-iteration value.
TEST(PassesTransformation_VariableFolding, vf_loop_stale0) { TEST_CORE; }
// A zero-trip for loop (constant-false condition) executes nothing: the body
// must not run even once before the first condition test.
TEST(PassesTransformation_VariableFolding, vf_for_zero0) { TEST_CORE; }
// A while whose *initial* condition is non-constant is left intact and all
// interpreter state is dropped: a later read must not fold to the pre-loop value.
TEST(PassesTransformation_VariableFolding, vf_while_nonconst0) { TEST_CORE; }
// Same conservative give-up for a repeat whose count is non-constant.
TEST(PassesTransformation_VariableFolding, vf_repeat_nonconst0) { TEST_CORE; }
// An if whose condition is non-constant may assign anything: reads after it
// must not fold to the pre-if value.
TEST(PassesTransformation_VariableFolding, vf_if_nonconst0) { TEST_CORE; }
// A partial-value write (bit-select lvalue) makes the whole variable unknown:
// a later whole-variable read must not fold to the pre-write value.
TEST(PassesTransformation_VariableFolding, vf_partial_write0) { TEST_CORE; }
// A task-call actual may be an output writeback target: it must not be folded
// to a constant, and reads after the call must not fold to pre-call values.
TEST(PassesTransformation_VariableFolding, vf_task_arg0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, for1) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, for2) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, gray2bin) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, initial_test0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, sv_variable_folding0) { TEST_CORE_SV; }
// Statements a consumed break makes unreachable (the tail of its own branch)
// must be truncated from the unrolled output, not emitted as live code (§12.8).
TEST(PassesTransformation_VariableFolding, vf_loop_jump_trunc0) { TEST_CORE_SV; }

namespace
{
bool contains_node_type(const AST::Node::Ptr &node, AST::NodeType type)
{
    if(!node) {
        return false;
    }
    if(node->is_node_type(type)) {
        return true;
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(contains_node_type(child, type)) {
            return true;
        }
    }
    return false;
}
} // namespace

// The unroll cap is configurable; a loop whose trip count exceeds it is left
// intact rather than unrolled.
TEST(PassesTransformation_VariableFolding, unroll_cap0)
{
    ENABLE_LOGGER;

    Parser::Verilog verilog;
    verilog.parse(test_helpers.get_verilog_filename("while0"));
    AST::Node::Ptr source = verilog.get_source();
    ASSERT_TRUE(source != nullptr);

    Passes::Transformations::VariableFolding::set_max_unroll_iterations(2);
    Passes::Transformations::VariableFolding().run(source);
    Passes::Transformations::VariableFolding::set_max_unroll_iterations(100000);

    ASSERT_AST_IS_TREE(source);
    ASSERT_TRUE(contains_node_type(source, AST::NodeType::WhileStatement));

    // Default cap: the same 5-iteration loop unrolls fully.
    Parser::Verilog verilog2;
    verilog2.parse(test_helpers.get_verilog_filename("while0"));
    AST::Node::Ptr source2 = verilog2.get_source();
    ASSERT_TRUE(source2 != nullptr);
    Passes::Transformations::VariableFolding().run(source2);
    ASSERT_FALSE(contains_node_type(source2, AST::NodeType::WhileStatement));
}
