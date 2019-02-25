#include "../../helpers/helpers.hpp"
#include <veriparse/AST/nodes.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/passes/transformations/annotate_scope.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/passes/transformations/testcases/");

#define TEST_CORE                                                                     \
	ENABLE_LOGGER;                                                                     \
                                                                                      \
	/* Parse the file to transform */                                                  \
	Parser::Verilog verilog;                                                           \
	verilog.parse(test_helpers.get_verilog_filename(test_name));                       \
	AST::Node::Ptr source = verilog.get_source();                                      \
	ASSERT_TRUE(source != nullptr);                                                    \
                                                                                      \
	/* apply the transformation */                                                     \
	test_helpers.render_node_to_verilog_file(source, test_string + "_before.v");       \
	Passes::Transformations::AnnotateScope ad("^.*$", "prefix_$&_suffix");             \
	ad.run(source);                                                                    \
	test_helpers.render_node_to_verilog_file(source, test_string + ".v");              \
	test_helpers.render_node_to_yaml_file(source, test_string + ".yaml");              \
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");                \
                                                                                      \
	/* load the reference */                                                           \
	std::string test_ref_suffix = "annotate_scope_";                                   \
	const std::string ref_filename = test_ref_suffix + test_name;                      \
	AST::Node::Ptr source_ref = Importers::YAMLImporter().import                       \
		(test_helpers.get_yaml_filename(ref_filename).c_str());                         \
	test_helpers.render_node_to_verilog_file(source_ref, test_string + "_ref.v");      \
	ASSERT_TRUE(source_ref != nullptr);                                                \
                                                                                      \
	/* Check parsed against reference */                                               \
	ASSERT_TRUE(source_ref->is_equal(*source, false))


TEST(PassesTransformation_AnnotateScope, generate1) {TEST_CORE;}
