#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/function_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>


using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");


TEST(PassesTransformation_FunctionEvaluation, function2)
{
	ENABLE_LOGGER;

	/* Parse the file to transform */
	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	/* Render existing module */
	test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");
	Passes::Analysis::Module::ModulesMap modules_map;
	Passes::Analysis::Module::get_module_dictionary(source, modules_map);

	/* List function calls */
	AST::FunctionCall::ListPtr functioncalls =
		Passes::Analysis::Module::get_functioncall_nodes(modules_map[test_name]);
	ASSERT_TRUE(functioncalls->size() == 1);
	const auto &functioncall = functioncalls->front();

	/* List all function declarations */
	AST::Function::ListPtr functions =
		Passes::Analysis::Module::get_function_nodes(modules_map[test_name]);

	Passes::Transformations::FunctionEvaluation::FunctionMap function_map;
	for (const auto &function: *functions) {
		function_map[function->get_name()] = function;
	}

	/* Process the function call */
	Passes::Transformations::FunctionEvaluation function_eval(1);
	const auto &node_result = function_eval.evaluate(functioncall, function_map);
	ASSERT_TRUE(node_result != nullptr);

	test_helpers.render_node_to_dot_file(node_result, test_string + ".dot");
	test_helpers.render_node_to_yaml_file(node_result, test_string + ".yaml");
	test_helpers.render_node_to_verilog_file(node_result, test_string + ".v");

	/* load the ref and check */
	std::string test_ref_suffix = "refs/function_evaluation_";
	const std::string ref_filename = test_ref_suffix + test_name;
	AST::Node::Ptr node_ref = Importers::YAMLImporter().import
		(test_helpers.get_yaml_filename(ref_filename).c_str());
	ASSERT_TRUE(node_ref != nullptr);
	ASSERT_TRUE(node_ref->is_equal(*node_result, false));
}
