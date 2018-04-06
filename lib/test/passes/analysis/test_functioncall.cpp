#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/functioncall.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("../../test/parser/testcases/");


TEST(PassesAnalysis_FunctionCall, expression42) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::FunctionCall::ListPtr functioncall_list;
	functioncall_list = Passes::Analysis::Module::get_functioncall_nodes(source);
	ASSERT_EQ(1u, functioncall_list->size());

	AST::FunctionCall::Ptr functioncall = functioncall_list->front();

	AST::Identifier::ListPtr identifiers;
	identifiers = Passes::Analysis::FunctionCall::get_argument_identifier_nodes(functioncall);
	ASSERT_EQ(1u, identifiers->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"in"};
	names = Passes::Analysis::FunctionCall::get_argument_identifier_names(functioncall);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_FunctionCall, functioncall0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::FunctionCall::ListPtr functioncall_list;
	functioncall_list = Passes::Analysis::Module::get_functioncall_nodes(source);
	ASSERT_EQ(2u, functioncall_list->size());

	// First function
	AST::FunctionCall::Ptr functioncall = functioncall_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::FunctionCall::get_argument_identifier_nodes(functioncall);
	ASSERT_EQ(4u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names{"in0", "in1", "in2", "in3"};
	fct1_names = Passes::Analysis::FunctionCall::get_argument_identifier_names(functioncall);
	ASSERT_EQ(4u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);

	// Second function
	functioncall_list->pop_front();
	functioncall = functioncall_list->front();

	AST::Identifier::ListPtr fct2_identifiers;
	fct2_identifiers = Passes::Analysis::FunctionCall::get_argument_identifier_nodes(functioncall);
	ASSERT_EQ(1u, fct2_identifiers->size());

	std::vector<std::string> fct2_names;
	std::vector<std::string> fct2_ref_names{"in3"};
	fct2_names = Passes::Analysis::FunctionCall::get_argument_identifier_names(functioncall);
	ASSERT_EQ(1u, fct2_names.size());
	ASSERT_EQ(fct2_ref_names, fct2_names);
}
