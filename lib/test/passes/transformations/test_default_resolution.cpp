// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/default_resolution.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE_MODE(sv_mode, source_filename)                                                   \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(sv_mode);                                                                  \
    verilog.parse(source_filename);                                                                \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* apply the transformation */                                                                 \
    Passes::Transformations::DefaultResolution resolution(sv_mode);                                \
    ASSERT_EQ(0, resolution.run(source));                                                          \
    ASSERT_AST_IS_TREE(source); /* pass output must stay a proper tree */                          \
    test_helpers.render_node_to_verilog_file(source, test_string + ".v");                          \
    test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");                          \
    test_helpers.render_node_to_dot_file(source, test_string + ".dot");                            \
                                                                                                   \
    /* load the reference */                                                                       \
    std::string test_ref_suffix = "refs/default_resolution_";                                      \
    std::string ref_filename = test_ref_suffix + test_name;                                        \
    AST::Node::Ptr source_ref =                                                                    \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(source_ref != nullptr);                                                            \
                                                                                                   \
    /* Check the resolved AST against the reference */                                             \
    ASSERT_TRUE(source_ref->is_equal(*source, false))

#define TEST_CORE_SV TEST_CORE_MODE(true, test_helpers.get_sv_filename(test_name))
#define TEST_CORE_VERILOG TEST_CORE_MODE(false, test_helpers.get_verilog_filename(test_name))

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
    Passes::Transformations::DefaultResolution resolution(true);                                   \
    ASSERT_NE(0, resolution.run(source))

// Implicit data types on variables, parameters (§6.20.2 carve-out),
// nets, and subroutine returns/args.
TEST(PassesTransformation_DefaultResolution, default_resolution0) { TEST_CORE_SV; }
// ANSI ports with explicit directions: §23.2.2.3 kind rules.
TEST(PassesTransformation_DefaultResolution, default_resolution1) { TEST_CORE_SV; }
// Per-module `default_nettype scoping.
TEST(PassesTransformation_DefaultResolution, default_resolution2) { TEST_CORE_SV; }
// Non-ANSI: unbacked direction declarations resolve, backed ones stay.
TEST(PassesTransformation_DefaultResolution, default_resolution3) { TEST_CORE_SV; }
// Interface: ports resolve like a module's; interface ports of a module
// stay untouched.
TEST(PassesTransformation_DefaultResolution, default_resolution4) { TEST_CORE_SV; }
// The §23.2.2.3 mh examples that parse (ADR-0012 §3): first-port inout
// default, direction inheritance, kind rules, per the LRM's comments.
TEST(PassesTransformation_DefaultResolution, default_resolution_mh0) { TEST_CORE_SV; }
// Old-style task/function formals are variables (§13.3/§13.4): the port
// rules stop at the subroutine boundary — no net kind, no `none error.
TEST(PassesTransformation_DefaultResolution, default_resolution_tf0) { TEST_CORE_SV; }
// `default_nettype none: an implicit net is a hard error (§22.8).
TEST(PassesTransformation_DefaultResolution, default_resolution_none0) { TEST_ERROR_SV; }
// Verilog mode: net kinds resolve, implicit types stay (1364 in, 1364 out).
TEST(PassesTransformation_DefaultResolution, default_resolution_verilog0) { TEST_CORE_VERILOG; }
