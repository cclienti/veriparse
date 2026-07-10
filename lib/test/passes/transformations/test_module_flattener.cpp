// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/logger/logger.hpp>
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
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");                     \
    /* apply the transformation */                                                                 \
    Passes::Transformations::ModuleFlattener flattener(AST::ParamArg::ListPtr(), modules_map);     \
    Passes::Analysis::UniqueDeclaration::seed(0);                                                  \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    flattener.run(modules_map[test_name]);                                                         \
    ASSERT_AST_IS_TREE(modules_map[test_name]); /* pass output must stay a proper tree */          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/module_flattener_";                                        \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");                  \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

// SystemVerilog cases (interfaces): parse in SV mode, resolve names so
// interface instances/ports are tagged, then flatten with the interface
// dictionary.
#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    Passes::Transformations::NameResolution resolution;                                            \
    ASSERT_EQ(0, resolution.run(source));                                                          \
                                                                                                   \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    Passes::Analysis::Module::InterfacesMap interfaces_map;                                        \
    Passes::Analysis::Module::get_interface_dictionary(source, interfaces_map);                    \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.sv");                  \
    Passes::Transformations::ModuleFlattener flattener(AST::ParamArg::ListPtr(), modules_map,      \
                                                       true, interfaces_map);                      \
    Passes::Analysis::UniqueDeclaration::seed(0);                                                  \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    ASSERT_EQ(0, flattener.run(modules_map[test_name]));                                           \
    ASSERT_AST_IS_TREE(modules_map[test_name]); /* pass output must stay a proper tree */          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".sv");                         \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/module_flattener_";                                        \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.sv");                 \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

// Illegal-input cases: the flattener must reject the design (non-zero).
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
    ASSERT_EQ(0, resolution.run(source));                                                          \
                                                                                                   \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    Passes::Analysis::Module::InterfacesMap interfaces_map;                                        \
    Passes::Analysis::Module::get_interface_dictionary(source, interfaces_map);                    \
                                                                                                   \
    Passes::Transformations::ModuleFlattener flattener(AST::ParamArg::ListPtr(), modules_map,      \
                                                       true, interfaces_map);                      \
    Passes::Analysis::UniqueDeclaration::seed(0);                                                  \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    ASSERT_NE(0, flattener.run(modules_map[test_name]))

TEST(PassesTransformation_ModuleFlattener, instance0) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance1) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance2) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance3) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance4) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance5) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance6) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance7) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance8) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, defparam4) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, deadcode6) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, scoped0) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, instance_array_scoped0) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, shmemif) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, alu_dsp) { TEST_CORE; }

// Interface elaboration (ADR-0008 §2): interface instances flatten as
// pseudo-modules; direct member accesses resolve to the flattened signals.
TEST(PassesTransformation_ModuleFlattener, iface_inst0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_header0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_param0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_body0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_module_in_iface0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_top_port0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_bad_modport0) { TEST_ERROR_SV; }

// A SystemVerilog logic variable accepts a child-output connection
// (continuous assignment to a variable, IEEE 1800-2017 §10.3.2).
TEST(PassesTransformation_ModuleFlattener, logic_out0) { TEST_CORE_SV; }

// Interface ports on children (ADR-0008 §3/§4): references through the port
// alias the connected instance's flattened signals.
TEST(PassesTransformation_ModuleFlattener, iface_port0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_modport0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_modport1) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_chain0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_out0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_hdr_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_conn_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_modport_mismatch0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_not_in_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_no_member0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_unconnected0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_type_mismatch0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested_path0) { TEST_ERROR_SV; }
