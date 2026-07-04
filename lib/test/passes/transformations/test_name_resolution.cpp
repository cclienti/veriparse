// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/logger/logger.hpp>
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
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.sv");                  \
    /* apply the transformation */                                                                 \
    Passes::Transformations::NameResolution resolution;                                            \
    ASSERT_EQ(0, resolution.run(source));                                                          \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".sv");                         \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/name_resolution_";                                         \
    std::string ref_filename = test_ref_suffix + test_name;                                        \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

// Illegal-input cases: the pass must reject the source (non-zero), no golden.
#define TEST_ERROR_SV                                                                              \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    Passes::Transformations::NameResolution resolution;                                            \
    ASSERT_NE(0, resolution.run(source))

TEST(PassesTransformation_NameResolution, name_resolution0) { TEST_CORE_SV; }
TEST(PassesTransformation_NameResolution, name_resolution1) { TEST_CORE_SV; }
TEST(PassesTransformation_NameResolution, name_resolution2) { TEST_CORE_SV; }
TEST(PassesTransformation_NameResolution, name_resolution3) { TEST_ERROR_SV; }
TEST(PassesTransformation_NameResolution, name_resolution4) { TEST_ERROR_SV; }
