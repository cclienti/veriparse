#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
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
	Passes::Analysis::Module::ModulesMap modules_map;                            \
	Passes::Analysis::Module::get_module_dictionary(source, modules_map);        \
                                                                                \
	test_helpers.render_node_to_verilog_file(source, test_string + "_before.v"); \
	test_helpers.render_node_to_dot_file(source, test_string + "_before.dot");   \
	/* apply the transformation */                                               \
	Passes::Transformations::ModuleInstanceNormalizer normalize(modules_map);    \
	ASSERT_TRUE(modules_map.count(test_name) == 1);                              \
	normalize.run(modules_map[test_name]);                                       \
	test_helpers.render_node_to_verilog_file(source, test_string + ".v");        \
	test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");        \
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");          \
                                                                                \
	/* load the reference */                                                     \
	std::string test_ref_suffix = "refs/module_instance_normalizer_";            \
	const std::string ref_filename = test_ref_suffix + test_name;                \
	AST::Node::Ptr source_ref = Importers::YAMLImporter().import                 \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                   \
	ASSERT_TRUE(source_ref != nullptr);                                          \
                                                                                \
	/* Check parsed against reference */                                         \
	ASSERT_TRUE(source_ref->is_equal(*source, false))


TEST(PassesTransformation_ModuleInstanceNormalizer, instance0) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance1) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance2) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance3) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance4) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance5) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance6) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance7) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance8) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance9) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance10) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, instance11) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, defparam0) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, defparam3) {TEST_CORE;}
TEST(PassesTransformation_ModuleInstanceNormalizer, defparam4) {TEST_CORE;}
