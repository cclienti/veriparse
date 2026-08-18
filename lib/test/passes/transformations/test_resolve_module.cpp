// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
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
    Passes::Transformations::ResolveModule resolve(AST::ParamArg::ListPtr(), modules_map);         \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    resolve.run(modules_map[test_name]);                                                           \
    ASSERT_AST_IS_TREE(modules_map[test_name]); /* pass output must stay a proper tree */          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/resolve_module_";                                          \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");                  \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check parsed against reference */                                                           \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

#define TEST_CORE_FSM_SV                                                                           \
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
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    Passes::Transformations::ResolveModule resolve(AST::ParamArg::ListPtr(), modules_map, true,    \
                                                   true);                                          \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    ASSERT_EQ(0, resolve.run(modules_map[test_name]));                                             \
    ASSERT_AST_IS_TREE(modules_map[test_name]);                                                    \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
                                                                                                   \
    std::string test_ref_suffix = "refs/resolve_module_";                                          \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

// Same as TEST_CORE_FSM_SV without the FSM slot, with an interface beside
// the module and NameResolution + the interface dictionary feeding
// ResolveModule — the ADR-0015 §3.1 seat of HierCallResolution, so the
// spliced clone rides every later pass exactly like local text.
#define TEST_CORE_IFACE_SV                                                                         \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    ASSERT_EQ(0, Passes::Transformations::NameResolution().run(source));                           \
                                                                                                   \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    Passes::Analysis::Module::InterfacesMap interfaces_map;                                        \
    Passes::Analysis::Module::get_interface_dictionary(source, interfaces_map);                    \
                                                                                                   \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    Passes::Transformations::ResolveModule resolve(AST::ParamArg::ListPtr(), modules_map, true,    \
                                                   false, nullptr, interfaces_map);                \
    ASSERT_TRUE(modules_map.count(test_name) == 1);                                                \
    ASSERT_EQ(0, resolve.run(modules_map[test_name]));                                             \
    ASSERT_AST_IS_TREE(modules_map[test_name]);                                                    \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
                                                                                                   \
    std::string test_ref_suffix = "refs/resolve_module_";                                          \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

TEST(PassesTransformation_ResolveModule, scope0) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, scope1) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, scope2) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, module1) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, module2) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, shmemif) { TEST_CORE; }
TEST(PassesTransformation_ResolveModule, alu_dsp) { TEST_CORE; }
// The §10.3 slot end to end: LoopUnrolling flattens the unmarked repeat into
// two waits before the FSM lowering cuts, LocalparamInliner has already fed
// the constant into a segment, and the folding passes clean the generated
// machine (ADR-0014 §10.3).
TEST(PassesTransformation_ResolveModule, resolve_fsm0) { TEST_CORE_FSM_SV; }
// §7.2 unrolled groups end to end: an unmarked bounded for with a cut point
// is replicated by LoopUnrolling before the lowering cuts — one state per
// copy, each reading its constant index — and the folding passes clean the
// machine (ADR-0014 §7.2, §12 phase 5).
TEST(PassesTransformation_ResolveModule, resolve_fsm1) { TEST_CORE_FSM_SV; }
// A spliced body holding an inline enum and a bounded for: the enum
// elaboration and loop unrolling that already ran must run again over the
// clone, or the raw constructs reach the output (ADR-0015 §3.1).
TEST(PassesTransformation_ResolveModule, resolve_hier_enum0) { TEST_CORE_IFACE_SV; }
// A spliced constant function folds like a local one: the function
// dictionary is rebuilt after the splice, no spurious not-found error.
TEST(PassesTransformation_ResolveModule, resolve_hier_fold0) { TEST_CORE_IFACE_SV; }
// A zero-argument call has no args list: constant evaluation treats it as
// empty (it used to crash) and folds.
TEST(PassesTransformation_ResolveModule, resolve_zeroarg0) { TEST_CORE_IFACE_SV; }
