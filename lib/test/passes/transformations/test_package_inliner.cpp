// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE_SV                                                                               \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    /* Parse the SV file (package + module in one compilation unit) */                             \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* Apply the pass on the whole source */                                                       \
    test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");                   \
    Passes::Transformations::PackageInliner().run(source);                                         \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* Load the reference */                                                                       \
    std::string ref_filename = std::string("refs/package_inliner_") + test_name;                   \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
    test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");                  \
                                                                                                   \
    /* Check */                                                                                    \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

/* Error cases: the pass must report a non-zero status (undefined package /
 * symbol, multi-wildcard ambiguity) rather than silently mis-resolving. */
#define TEST_ERROR_SV                                                                              \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_sv_filename(test_name));                                        \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    ASSERT_NE(Passes::Transformations::PackageInliner().run(source), 0)

TEST(PassesTransformation_PackageInliner, package_inliner0) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner1) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner2) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner3) { TEST_CORE_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner4) { TEST_CORE_SV; }

TEST(PassesTransformation_PackageInliner, package_inliner_undef_pkg) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_undef_sym) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_ambiguous) { TEST_ERROR_SV; }
TEST(PassesTransformation_PackageInliner, package_inliner_collision) { TEST_ERROR_SV; }

// Multi-source: a package defined in one file, referenced from a module in
// another (the veriflat model — global collect, per-unit resolve). Proves a
// package name crosses files while a top-level import does not.
TEST(PassesTransformation_PackageInliner, package_inliner_multi)
{
    ENABLE_LOGGER;

    Parser::Verilog va;
    va.set_sv_mode(true);
    va.parse(test_helpers.get_sv_filename("package_inliner_multi_a"));
    AST::Node::Ptr src_a = va.get_source();
    ASSERT_TRUE(src_a != nullptr);

    Parser::Verilog vb;
    vb.set_sv_mode(true);
    vb.parse(test_helpers.get_sv_filename("package_inliner_multi_b"));
    AST::Node::Ptr src_b = vb.get_source();
    ASSERT_TRUE(src_b != nullptr);

    // Global collect across both units, then resolve each unit on its own.
    Passes::Transformations::PackageInliner inliner;
    ASSERT_EQ(inliner.collect(src_a), 0);
    ASSERT_EQ(inliner.collect(src_b), 0);
    ASSERT_EQ(inliner.resolve(src_a), 0);
    ASSERT_EQ(inliner.resolve(src_b), 0);
    ASSERT_AST_IS_TREE(src_a);
    ASSERT_AST_IS_TREE(src_b);

    // Golden on unit B: `xpkg::XW` (package from file A) is inlined here.
    test_helpers.render_node_to_verilog_file(src_b, test_string + ".v");
    test_helpers.render_node_to_yaml_file(src_b, test_string + ".yaml");
    test_helpers.render_node_to_dot_file(src_b, test_string + ".dot");
    std::string ref_filename = std::string("refs/package_inliner_") + test_name;
    AST::Node::Ptr ref =
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());
    ASSERT_TRUE(ref != nullptr);
    ASSERT_TRUE(ref->is_equal(*src_b, false));
}
