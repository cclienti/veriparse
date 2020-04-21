#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE                                                                \
	ENABLE_LOGGER;                                                                \
                                                                                 \
	Parser::Verilog verilog;                                                      \
	verilog.parse(test_helpers.get_verilog_filename(test_name));                  \
	AST::Node::Ptr source = verilog.get_source();                                 \
	ASSERT_TRUE(source != nullptr);                                               \
                                                                                 \
	Passes::Analysis::Module::ModulesMap modules_map;                             \
	Passes::Analysis::Module::get_module_dictionary(source, modules_map);         \
                                                                                 \
	test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");  \
	test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");    \
	/* apply the transformation */                                                \
	Passes::Transformations::ModuleFlattener flattener(AST::ParamArg::ListPtr(),  \
	                                                   modules_map);              \
	Passes::Analysis::UniqueDeclaration::seed(0);                                 \
	ASSERT_TRUE(modules_map.count(test_name) == 1);                               \
	flattener.run(modules_map[test_name]);                                        \
	test_helpers.render_node_to_verilog_file(source, test_string + ".v");         \
	test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");         \
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");           \
	                                                                              \
	/* load the reference */                                                      \
	std::string test_ref_suffix = "refs/module_flattener_";                       \
	const std::string ref_filename = test_ref_suffix + test_name;                 \
	AST::Node::Ptr source_ref = Importers::YAMLImporter().import                  \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                    \
	test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v"); \
	ASSERT_TRUE(source_ref != nullptr);                                           \
                                                                                 \
	/* Check parsed against reference */                                          \
	ASSERT_TRUE(source_ref->is_equal(*source, false))


TEST(PassesTransformation_ModuleFlattener, instance0) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance1) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance2) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance3) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance4) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance5) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance6) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance7) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, instance8) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, defparam4) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, deadcode6) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, scoped0) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, shmemif) {TEST_CORE;}
TEST(PassesTransformation_ModuleFlattener, alu_dsp) {TEST_CORE;}
