#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/systemcall.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("../../test/parser/testcases/");


TEST(PassesAnalysis_SystemCall, expression43) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::SystemCall::ListPtr systemcall_list;
	systemcall_list = Passes::Analysis::Module::get_systemcall_nodes(source);
	ASSERT_EQ(1u, systemcall_list->size());

	AST::SystemCall::Ptr systemcall = systemcall_list->front();

	AST::Identifier::ListPtr identifiers;
	identifiers = Passes::Analysis::SystemCall::get_argument_identifier_nodes(systemcall);
	ASSERT_EQ(0u, identifiers->size());
}

TEST(PassesAnalysis_SystemCall, systemcall0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::SystemCall::ListPtr systemcall_list;
	systemcall_list = Passes::Analysis::Module::get_systemcall_nodes(source);
	ASSERT_EQ(1u, systemcall_list->size());

	// First system
	AST::SystemCall::Ptr systemcall = systemcall_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::SystemCall::get_argument_identifier_nodes(systemcall);
	ASSERT_EQ(4u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names{"in1", "in2", "in0", "in3"};
	fct1_names = Passes::Analysis::SystemCall::get_argument_identifier_names(systemcall);
	ASSERT_EQ(4u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);
}
