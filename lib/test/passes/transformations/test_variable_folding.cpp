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
TEST(PassesTransformation_VariableFolding, for1) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, for2) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, gray2bin) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, initial_test0) { TEST_CORE; }
TEST(PassesTransformation_VariableFolding, sv_variable_folding0) { TEST_CORE_SV; }
