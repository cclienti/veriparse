// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/hier_call_resolution.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

/* The pass runs after NameResolution (the interface-port promotion to
 * InterfaceType, ADR-0003 §4.4) with the design's interface dictionary in
 * hand, as ResolveModule provides both (ADR-0015 §3.1). */
#define TEST_SETUP_SV                                                                              \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    ASSERT_EQ(Passes::Transformations::NameResolution().run(source), 0);                           \
                                                                                                   \
    Passes::Analysis::Module::InterfacesMap interfaces_map;                                        \
    ASSERT_EQ(Passes::Analysis::Module::get_interface_dictionary(source, interfaces_map), 0)

#define TEST_CORE_SV                                                                               \
    TEST_SETUP_SV;                                                                                 \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    ASSERT_EQ(Passes::Transformations::HierCallResolution(interfaces_map).run(source), 0);         \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* Load the reference */                                                                       \
    std::string ref_filename = std::string("refs/hier_call_resolution_") + test_name;              \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");                  \
                                                                                                   \
    /* Check */                                                                                    \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

/* Error cases: rejected loudly, never silently mis-lowered (ADR-0015 §6). */
#define TEST_ERROR_SV                                                                              \
    TEST_SETUP_SV;                                                                                 \
                                                                                                   \
    ASSERT_NE(Passes::Transformations::HierCallResolution(interfaces_map).run(source), 0)

// The splice through a port: two ping() sites resolve to one spliced task, a
// function call in expression and the formal-shadows-member case.
TEST(PassesTransformation_HierCallResolution, hier_call_port0) { TEST_CORE_SV; }

// The root names a module instance.
TEST(PassesTransformation_HierCallResolution, hier_call_err_root0) { TEST_ERROR_SV; }
// The root names a local interface instance (ADR-0015 §3.2 collision).
TEST(PassesTransformation_HierCallResolution, hier_call_err_root1) { TEST_ERROR_SV; }
// A multi-level path.
TEST(PassesTransformation_HierCallResolution, hier_call_err_multi0) { TEST_ERROR_SV; }
// The interface declares no such subroutine.
TEST(PassesTransformation_HierCallResolution, hier_call_err_missing0) { TEST_ERROR_SV; }
// A task called in expression position (IEEE 1800-2017 §13.5).
TEST(PassesTransformation_HierCallResolution, hier_call_err_exprtask0) { TEST_ERROR_SV; }
// The body references an interface localparam (v1 closure).
TEST(PassesTransformation_HierCallResolution, hier_call_err_param0) { TEST_ERROR_SV; }
// The body calls a sibling interface subroutine (v1 closure).
TEST(PassesTransformation_HierCallResolution, hier_call_err_nested0) { TEST_ERROR_SV; }
// An indexed root has no static identity.
TEST(PassesTransformation_HierCallResolution, hier_call_err_indexed0) { TEST_ERROR_SV; }
