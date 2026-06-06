// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/passes/transformations/enum_elaboration.hpp>
#include <veriparse/passes/transformations/enum_inliner.hpp>
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
    /* Parse the SV file */                                                                        \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* Get the module */                                                                           \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    const auto &module = modules_map[test_name];                                                   \
                                                                                                   \
    /* Function map for constant folding */                                                        \
    Passes::Analysis::Module::FunctionMap function_map;                                            \
    Passes::Analysis::Module::get_function_dictionary(module, function_map);                       \
                                                                                                   \
    /* Apply the pipeline */                                                                       \
    test_helpers.render_node_to_verilog_file(module, test_string + "_before.v");                   \
    Passes::Transformations::ConstantFolding(function_map).run(module);                            \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    Passes::Transformations::EnumElaboration().run(module);                                        \
    Passes::Transformations::EnumInliner().run(module);                                            \
    ASSERT_AST_IS_TREE(module); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(module, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(module, test_string + ".dot");                            \
                                                                                                   \
    /* Load the reference */                                                                       \
    std::string ref_filename = std::string("refs/enum_inliner_") + test_name;                      \
    AST::Node::Ptr module_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(module_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(module_ref, test_string + "_ref.v");                  \
                                                                                                   \
    /* Check */                                                                                    \
    ASSERT_TRUE(module_ref->is_equal(*module, false))

TEST(PassesTransformation_EnumInliner, enum_inliner0) { TEST_CORE_SV; }
TEST(PassesTransformation_EnumInliner, enum_inliner1) { TEST_CORE_SV; }
