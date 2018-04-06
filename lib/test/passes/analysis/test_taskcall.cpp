#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/taskcall.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("../../test/parser/testcases/");


TEST(PassesAnalysis_TaskCall, width1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::TaskCall::ListPtr taskcall_list;
	taskcall_list = Passes::Analysis::Module::get_taskcall_nodes(source);
	ASSERT_EQ(1u, taskcall_list->size());

	AST::TaskCall::Ptr taskcall = taskcall_list->front();

	AST::Identifier::ListPtr identifiers;
	identifiers = Passes::Analysis::TaskCall::get_argument_identifier_nodes(taskcall);
	ASSERT_EQ(2u, identifiers->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"address", "MEM_WORD"};
	names = Passes::Analysis::TaskCall::get_argument_identifier_names(taskcall);
	ASSERT_EQ(2u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_TaskCall, taskcall0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::TaskCall::ListPtr taskcall_list;
	taskcall_list = Passes::Analysis::Module::get_taskcall_nodes(source);
	ASSERT_EQ(1u, taskcall_list->size());

	// First task
	AST::TaskCall::Ptr taskcall = taskcall_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::TaskCall::get_argument_identifier_nodes(taskcall);
	ASSERT_EQ(4u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names{"in1", "in2", "in0", "in3"};
	fct1_names = Passes::Analysis::TaskCall::get_argument_identifier_names(taskcall);
	ASSERT_EQ(4u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);
}
