#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/lvalue.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;
using namespace Passes;

static TestHelpers test_helpers("../../test/parser/testcases/");
static TestHelpers test2_helpers("../../test/passes/analysis/testcases/");

TEST(PassesAnalysis_Lvalue, lvalue0)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(1u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"b", "c"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"a"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, lvalue1)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(1u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"b", "c"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"a"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, lvalue2)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(1u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"b0", "c", "b1", "c"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"a", "a"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, lvalue3)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(6u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"j", "i"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"i", "i", "j", "j", "tmp", "b"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, lvalue4)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(6u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"j", "i"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"j", "j", "i", "i", "tmp", "b"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, lvalue5)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(6u, lv_nodes->size());

	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_rv_id_names{"j", "i"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);

	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"j", "j", "i", "i", "tmp", "b"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		std::vector<std::string> lv_names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(lv_names), std::end(lv_names));
	}
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, expression37)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Lvalue::ListPtr lv_nodes;
	lv_nodes = Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(1u, lv_nodes->size());

	AST::Identifier::ListPtr lv_id_nodes = std::make_shared<AST::Identifier::List>();
	std::vector<std::string> lv_id_names;
	std::vector<std::string> lv_rv_id_names;
	std::vector<std::string> ref_lv_id_names{"out", "out"};
	std::vector<std::string> ref_lv_rv_id_names{"WIDTH", "WIDTH", "WIDTH"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		lv_id_nodes->splice(std::end(*lv_id_nodes), *Analysis::Lvalue::get_lvalue_nodes(lv));

		std::vector<std::string> names = Analysis::Lvalue::get_lvalue_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(names), std::end(names));

		std::vector<std::string> rv_names = Analysis::Lvalue::get_rvalue_names(lv);
		lv_rv_id_names.insert(std::end(lv_rv_id_names), std::begin(rv_names), std::end(rv_names));
	}
	ASSERT_EQ(2u, lv_id_nodes->size());
	ASSERT_EQ(2u, lv_id_names.size());
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
	ASSERT_EQ(ref_lv_rv_id_names, lv_rv_id_names);
}

TEST(PassesAnalysis_Lvalue, instance0)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Module::ListPtr module_nodes;
	module_nodes = Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	AST::Lvalue::ListPtr lv_nodes;
	AST::Module::Ptr instance0 = module_nodes->front();
	module_nodes->pop_front();
	AST::Module::Ptr mod = module_nodes->front();
	module_nodes->pop_front();
	lv_nodes = Analysis::Module::get_lvalue_nodes(mod);
	ASSERT_EQ(2u, lv_nodes->size());

	AST::Identifier::ListPtr lv_id_nodes = std::make_shared<AST::Identifier::List>();
	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"out", "out"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		lv_id_nodes->splice(std::end(*lv_id_nodes),
		                    *Analysis::Lvalue::get_identifier_nodes(lv));
		std::vector<std::string> cur = Analysis::Lvalue::get_identifier_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(cur), std::end(cur));
	}
	ASSERT_EQ(2u, lv_id_nodes->size());
	ASSERT_EQ(2u, lv_id_names.size());
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}

TEST(PassesAnalysis_Lvalue, instance1)
{
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Module::ListPtr module_nodes;
	module_nodes = Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	AST::Lvalue::ListPtr lv_nodes;
	AST::Module::Ptr instance0 = module_nodes->front();
	module_nodes->pop_front();
	AST::Module::Ptr mod = module_nodes->front();
	module_nodes->pop_front();
	lv_nodes = Analysis::Module::get_lvalue_nodes(mod);
	ASSERT_EQ(4u, lv_nodes->size());

	AST::Identifier::ListPtr lv_id_nodes = std::make_shared<AST::Identifier::List>();
	std::vector<std::string> lv_id_names;
	std::vector<std::string> ref_lv_id_names{"in0_ex", "in1_ex", "out", "out"};
	for(AST::Lvalue::Ptr lv: *lv_nodes) {
		lv_id_nodes->splice(std::end(*lv_id_nodes),
		                    *Analysis::Lvalue::get_identifier_nodes(lv));
		std::vector<std::string> cur = Analysis::Lvalue::get_identifier_names(lv);
		lv_id_names.insert(std::end(lv_id_names), std::begin(cur), std::end(cur));
	}
	ASSERT_EQ(4u, lv_id_nodes->size());
	ASSERT_EQ(4u, lv_id_names.size());
	ASSERT_EQ(ref_lv_id_names, lv_id_names);
}
