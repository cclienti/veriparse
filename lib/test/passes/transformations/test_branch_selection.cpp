#include "../../helpers/helpers.hpp"

#include <veriparse/passes/transformations/branch_selection.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/AST/nodes.hpp>

#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE                                                               \
	ENABLE_LOGGER;                                                               \
                                                                                \
	Parser::Verilog verilog;                                                     \
	verilog.parse(test_helpers.get_verilog_filename(test_name));                 \
	AST::Node::Ptr source = verilog.get_source();                                \
	ASSERT_TRUE(source != nullptr);                                              \
                                                                                \
	/* Get all modules in the file */                                            \
	Passes::Analysis::Module::ModulesMap modules_map;                            \
	Passes::Analysis::Module::get_module_dictionary(source, modules_map);	     \
	ASSERT_TRUE(modules_map.count(test_name) == 1);                              \
	const auto &module = modules_map[test_name];                                 \
                                                                                \
	/* Get all modules in the file */                                            \
	Passes::Analysis::Module::FunctionMap function_map;                          \
	Passes::Analysis::Module::get_function_dictionary(module, function_map);     \
                                                                                \
	/* apply the transformation */                                               \
	test_helpers.render_node_to_verilog_file(module, test_string + "_before.v"); \
	Passes::Transformations::BranchSelection(function_map).run(module);          \
	test_helpers.render_node_to_verilog_file(module, test_string + ".v");        \
	test_helpers.render_node_to_yaml_file(module, test_string + ".yaml");        \
	test_helpers.render_node_to_dot_file(module, test_string + ".dot");          \
                                                                                \
	/* load the reference */                                                     \
	std::string test_ref_suffix = "refs/branch_selection_";                      \
	const std::string ref_filename = test_ref_suffix + test_name;                \
	AST::Node::Ptr module_ref = Importers::YAMLImporter().import                 \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                   \
	ASSERT_TRUE(module_ref != nullptr);                                          \
                                                                                \
	/* Check parsed against reference */                                         \
	ASSERT_TRUE(module_ref->is_equal(*module, false))


TEST(PassesTransformation_BranchSelection, ifstmt0) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt1) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt3) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt4) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case0) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case1) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case2) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt_case0) {TEST_CORE;}
