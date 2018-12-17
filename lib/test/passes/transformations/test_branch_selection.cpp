#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/branch_selection.hpp>
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
	test_helpers.render_node_to_verilog_file(source, test_string + "_before.v"); \
	/* apply the transformation */                                               \
	Passes::Transformations::BranchSelection().run(source);                      \
	test_helpers.render_node_to_verilog_file(source, test_string + ".v");        \
	test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");        \
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");          \
                                                                                \
	/* load the reference */                                                     \
	std::string test_ref_suffix = "branch_selection_";                           \
	const std::string ref_filename = test_ref_suffix + test_name;                \
	AST::Node::Ptr source_ref = Importers::YAMLImporter().import                 \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                   \
	ASSERT_TRUE(source_ref != nullptr);                                          \
                                                                                \
	/* Check parsed against reference */                                         \
	ASSERT_TRUE(source_ref->is_equal(*source, false))


TEST(PassesTransformation_BranchSelection, ifstmt0) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt1) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt3) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case0) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case1) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, case2) {TEST_CORE;}
TEST(PassesTransformation_BranchSelection, ifstmt_case0) {TEST_CORE;}
