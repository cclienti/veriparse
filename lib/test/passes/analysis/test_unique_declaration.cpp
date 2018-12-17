#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;
using namespace Passes;

static TestHelpers test_helpers("lib/test/parser/testcases/");

TEST(PassesAnalysis_UniqueDeclaration, module3)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	Analysis::UniqueDeclaration::IdentifierSet id_set;
	Analysis::UniqueDeclaration::analyze(source, id_set);
	ASSERT_TRUE(id_set.size() != 0);

	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("module3", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("T1", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("T2", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("T4", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("A", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("B", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("C", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("X", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("Y", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("Z", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("reset", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("clk", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("in", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("out", id_set));
	ASSERT_FALSE(Analysis::UniqueDeclaration::identifier_declaration_exists("xx", id_set));

	const auto &unique0 = Analysis::UniqueDeclaration::get_unique_identifier("module3", id_set);
	ASSERT_TRUE(unique0.size() == 14);
	ASSERT_TRUE(unique0 != "module3");
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists(unique0, id_set));

	const auto &unique1 = Analysis::UniqueDeclaration::get_unique_identifier("really_unique", id_set);
	ASSERT_TRUE(unique1 == "really_unique");
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists(unique1, id_set));
}

TEST(PassesAnalysis_UniqueDeclaration, instance4)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	Analysis::UniqueDeclaration::IdentifierSet id_set;
	Analysis::UniqueDeclaration::analyze(source, id_set);
	ASSERT_TRUE(id_set.size() != 0);

	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("DFF", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("dffn", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("MxN_pipeline", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("p0", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("p1", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("dff", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("bits", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("M", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("q", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("d", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("t0", id_set));
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists("t1", id_set));
	ASSERT_FALSE(Analysis::UniqueDeclaration::identifier_declaration_exists("n", id_set));

	const auto &unique0 = Analysis::UniqueDeclaration::get_unique_identifier("p0", id_set);
	ASSERT_TRUE(unique0.size() == 9);
	ASSERT_TRUE(unique0 != "p0");
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists(unique0, id_set));

	const auto &unique1 = Analysis::UniqueDeclaration::get_unique_identifier("really_unique", id_set);
	ASSERT_TRUE(unique1 == "really_unique");
	ASSERT_TRUE(Analysis::UniqueDeclaration::identifier_declaration_exists(unique1, id_set));
}
