// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "../../helpers/helpers.hpp"
#include "veriparse/passes/analysis/function.hpp"

#include <veriparse/passes/transformations/function_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE_BODY(SVMODE)                                                                     \
    ENABLE_LOGGER;                                                                                 \
                                                                                                   \
    /* Parse the file to transform */                                                              \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(SVMODE);                                                                   \
    verilog.parse(test_helpers.get_verilog_filename(test_name));                                   \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
                                                                                                   \
    /* Render existing module */                                                                   \
    test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");                     \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
                                                                                                   \
    /* List function calls */                                                                      \
    AST::FunctionCall::ListPtr functioncalls =                                                     \
        Passes::Analysis::Module::get_functioncall_nodes(modules_map[test_name]);                  \
    ASSERT_TRUE(functioncalls->size() == 1);                                                       \
    const auto &functioncall = functioncalls->front();                                             \
                                                                                                   \
    /* Get the map of function definitions */                                                      \
    Passes::Analysis::Module::FunctionMap function_map;                                            \
    Passes::Analysis::Module::get_function_dictionary(modules_map[test_name], function_map);       \
                                                                                                   \
    /* Process the function call */                                                                \
    Passes::Transformations::FunctionEvaluation::set_max_recurse(16);                              \
    Passes::Transformations::FunctionEvaluation::reset_force_automatic();                          \
    Passes::Transformations::FunctionEvaluation function_eval;                                     \
    const auto &node_result = function_eval.evaluate(functioncall, function_map);                  \
    ASSERT_TRUE(node_result != nullptr);                                                           \
    ASSERT_AST_IS_TREE(node_result); /* evaluated result must be a proper tree */                  \
                                                                                                   \
    test_helpers.render_node_to_dot_file(node_result, test_string + ".dot");                       \
    test_helpers.render_node_to_yaml_file(node_result, test_string + ".yaml");                     \
    test_helpers.render_node_to_verilog_file(node_result, test_string + ".v");                     \
                                                                                                   \
    /* load the ref and check */                                                                   \
    std::string test_ref_suffix = "refs/function_evaluation_";                                     \
    const std::string ref_filename = test_ref_suffix + test_name;                                  \
    AST::Node::Ptr node_ref =                                                                      \
        Importers::YAMLImporter().import(test_helpers.get_yaml_filename(ref_filename).c_str());    \
    ASSERT_TRUE(node_ref != nullptr);                                                              \
    ASSERT_TRUE(node_ref->is_equal(*node_result, false))

#define TEST_CORE TEST_CORE_BODY(false)
#define TEST_CORE_SV TEST_CORE_BODY(true)

/* A call the evaluator must refuse to fold (returns nullptr) — e.g. a function
 * whose result depends on a data-dependent return the interpreter cannot resolve. */
#define TEST_NOFOLD_SV                                                                             \
    ENABLE_LOGGER;                                                                                 \
    Parser::Verilog verilog;                                                                       \
    verilog.set_sv_mode(true);                                                                     \
    verilog.parse(test_helpers.get_verilog_filename(test_name));                                   \
    AST::Node::Ptr source = verilog.get_source();                                                  \
    ASSERT_TRUE(source != nullptr);                                                                \
    Passes::Analysis::Module::ModulesMap modules_map;                                              \
    Passes::Analysis::Module::get_module_dictionary(source, modules_map);                          \
    AST::FunctionCall::ListPtr functioncalls =                                                     \
        Passes::Analysis::Module::get_functioncall_nodes(modules_map[test_name]);                  \
    ASSERT_TRUE(functioncalls->size() == 1);                                                       \
    Passes::Analysis::Module::FunctionMap function_map;                                            \
    Passes::Analysis::Module::get_function_dictionary(modules_map[test_name], function_map);       \
    Passes::Transformations::FunctionEvaluation::set_max_recurse(16);                              \
    Passes::Transformations::FunctionEvaluation::reset_force_automatic();                          \
    Passes::Transformations::FunctionEvaluation function_eval;                                     \
    ASSERT_TRUE(function_eval.evaluate(functioncalls->front(), function_map) == nullptr)

TEST(PassesTransformation_FunctionEvaluation, function2) { TEST_CORE; }
TEST(PassesTransformation_FunctionEvaluation, function3) { TEST_CORE; }
TEST(PassesTransformation_FunctionEvaluation, function4) { TEST_CORE; }
// A constant function that returns via `return expr;` (§12.8) — the interpreter
// treats it as the result and early-exits.
TEST(PassesTransformation_FunctionEvaluation, function_return0) { TEST_CORE_SV; }
// Early `return` unwinds: g(-3) takes `return 0;` and must NOT fall through to the
// later `return x*2;` (which would give -6). Verifies statements after a jump are
// not executed.
TEST(PassesTransformation_FunctionEvaluation, function_return1) { TEST_CORE_SV; }
// `break` at a statically-resolved condition ends the interpreted loop: f(3) sums
// 0+1+2+3 = 6 then breaks (§12.8).
TEST(PassesTransformation_FunctionEvaluation, function_break0) { TEST_CORE_SV; }
// `continue` skips the rest of one iteration: f(5) counts 0,1,3,4 = 4 (skips i==2).
TEST(PassesTransformation_FunctionEvaluation, function_continue0) { TEST_CORE_SV; }
// A data-dependent `return` (guard reads a non-argument signal) cannot be resolved,
// so the call must NOT be folded to a constant that ignores that exit (§12.8).
TEST(PassesTransformation_FunctionEvaluation, function_condret0) { TEST_NOFOLD_SV; }
