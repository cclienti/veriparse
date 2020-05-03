#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>


using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");


#define TEST_CORE                                                                \
	ENABLE_LOGGER;                                                                \
                                                                                 \
	/* Parse the file to transform */                                             \
	Parser::Verilog verilog;                                                      \
	verilog.parse(test_helpers.get_verilog_filename(test_name));                  \
	AST::Node::Ptr source = verilog.get_source();                                 \
	ASSERT_TRUE(source != nullptr);                                               \
                                                                                 \
	/* Get all modules in the file */                                             \
	Passes::Analysis::Module::ModulesMap modules_map;                             \
	Passes::Analysis::Module::get_module_dictionary(source, modules_map);	      \
	ASSERT_TRUE(modules_map.count(test_name) == 1);                               \
	const auto &module = modules_map[test_name];                                  \
                                                                                 \
	/* Get all modules in the file */                                             \
	Passes::Analysis::Module::FunctionMap function_map;                           \
	Passes::Analysis::Module::get_function_dictionary(module, function_map);      \
	                                                                              \
	/* apply the transformation */                                                \
	test_helpers.render_node_to_verilog_file(module, test_string + "_before.v");  \
	Passes::Transformations::ConstantFolding(function_map).run(module);           \
	test_helpers.render_node_to_verilog_file(module, test_string + ".v");         \
	test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");         \
	test_helpers.render_node_to_dot_file(module, test_string + ".dot");           \
                                                                                 \
	/* load the reference */                                                      \
	std::string test_ref_suffix = "refs/constant_folding_";                       \
	const std::string ref_filename = test_ref_suffix + test_name;                 \
	AST::Node::Ptr module_ref = Importers::YAMLImporter().import                  \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                    \
	ASSERT_TRUE(module_ref != nullptr);                                           \
	test_helpers.render_node_to_verilog_file(module_ref, test_string + "_ref.v"); \
                                                                                 \
	/* Check parsed against reference */                                          \
	ASSERT_TRUE(module_ref->is_equal(*module, false))


TEST(PassesTransformation_ConstantFolding, constant_folding0) {TEST_CORE;}
TEST(PassesTransformation_ConstantFolding, constant_folding1) {TEST_CORE;}
TEST(PassesTransformation_ConstantFolding, constant_folding2) {TEST_CORE;}
