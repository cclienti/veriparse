// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/passes/transformations/package_inliner.hpp>
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

// SystemVerilog cases (interfaces, packages, typedefs): parse in SV mode,
// inline packages and resolve names — the veriflat front-end order — then
// flatten with the interface dictionary.
#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    ASSERT_EQ(0, Passes::Transformations::PackageInliner().run(source));                           \
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

// Illegal-input Verilog cases: the flattener must reject the design (non-zero).
#define TEST_ERROR                                                                                 \
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
    Passes::Transformations::ModuleFlattener flattener(AST::ParamArg::ListPtr(), modules_map);     \
    Passes::Analysis::UniqueDeclaration::seed(0);                                                  \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    ASSERT_NE(0, flattener.run(modules_map[test_name]))

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
    ASSERT_EQ(0, Passes::Transformations::PackageInliner().run(source));                           \
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
// A runtime (non-constant) index into a split instance array cannot select a
// unique flattened element: the flattener rejects it instead of leaving a
// dangling hierarchical reference.
TEST(PassesTransformation_ModuleFlattener, hier_err_runtime_index0) { TEST_ERROR; }
// A child-output actual that is a hierarchical reference to a non-interface
// net is not a drivable local net: the flattener rejects it loudly.
TEST(PassesTransformation_ModuleFlattener, hier_err_out0) { TEST_ERROR; }
TEST(PassesTransformation_ModuleFlattener, shmemif) { TEST_CORE; }
TEST(PassesTransformation_ModuleFlattener, alu_dsp) { TEST_CORE; }

// Interface elaboration (ADR-0008 §2): interface instances flatten as
// pseudo-modules; direct member accesses resolve to the flattened signals.
TEST(PassesTransformation_ModuleFlattener, iface_inst0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_header0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_param0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_body0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_module_in_iface0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_top_port0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_bad_modport0) { TEST_ERROR_SV; }

// A SystemVerilog logic variable accepts a child-output connection
// (continuous assignment to a variable, IEEE 1800-2017 §10.3.2).
TEST(PassesTransformation_ModuleFlattener, logic_out0) { TEST_CORE_SV; }

// Typedef inlining (ADR-0009): typedefs substitute to concrete types before
// flattening, so the flat output carries no typedef and no NamedType.
TEST(PassesTransformation_ModuleFlattener, tdef_collision0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, tdef_port0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, tdef_pkg0) { TEST_CORE_SV; }
// The Dimensions-blindness repro: a typedef-typed actual on an instance
// array must split element-wise, not truncate to a shared scalar.
TEST(PassesTransformation_ModuleFlattener, tdef_array0) { TEST_CORE_SV; }

// Interface ports on children (ADR-0008 §3/§4): references through the port
// alias the connected instance's flattened signals.
TEST(PassesTransformation_ModuleFlattener, iface_port0) { TEST_CORE_SV; }
// Two ports where the second is named like the first port's actual: the
// single-pass rewrite must not re-capture the first rewrite's result.
TEST(PassesTransformation_ModuleFlattener, iface_alias_capture0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_modport0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_modport1) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_chain0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_out0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_hdr_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_conn_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_modport_mismatch0) { TEST_ERROR_SV; }
// A chained interface port declared with a bogus modport must be reported,
// not crash the modport lookup.
TEST(PassesTransformation_ModuleFlattener, iface_err_chained_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_not_in_modport0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_no_member0) { TEST_ERROR_SV; }
// An interface enumerator is a constant, not an aliasable signal: a reference
// through a port (p.EN_IDLE) is rejected.
TEST(PassesTransformation_ModuleFlattener, iface_err_enum_member0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_unconnected0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_type_mismatch0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested_path0) { TEST_ERROR_SV; }

// Interface arrays (ADR-0008 §5): element-wise, index-preserving connection.
TEST(PassesTransformation_ModuleFlattener, iface_array0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_array1) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_array_port0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_array_shared0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_elem0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_array_range0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_array_nonconst0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_array_noindex0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_array_bogus0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_member_actual0) { TEST_ERROR_SV; }
// Opposite-direction formal/actual array ranges (u[0:1] vs v[1:0]) require
// left-to-left remapping, deferred past v1: rejected as a range mismatch.
TEST(PassesTransformation_ModuleFlattener, iface_array_pairing0) { TEST_ERROR_SV; }

// Nested interfaces (ADR-0008 §8.1): recursive elaboration, structural
// port-path validation, nested actuals.
TEST(PassesTransformation_ModuleFlattener, iface_nested0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_nested_port0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_nested_actual0) { TEST_CORE_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested_cycle0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested_member0) { TEST_ERROR_SV; }
TEST(PassesTransformation_ModuleFlattener, iface_err_nested_modport0) { TEST_ERROR_SV; }
