#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases/");


TEST(PassesAnalysis_Instance, instance0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Module::ListPtr module_list = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_TRUE(module_list != nullptr);
	ASSERT_EQ(2u, module_list->size());


	AST::Instance::ListPtr instance_list;
	instance_list = Passes::Analysis::Module::get_instance_nodes(module_list->front());
	ASSERT_TRUE(instance_list != nullptr);
	ASSERT_EQ(1u, instance_list->size());

	// First system
	AST::Instance::Ptr instance = instance_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::Instance::get_argument_identifier_nodes(instance);
	ASSERT_EQ(6u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names{"clock", "reset", "in0_i", "in0_i", "in1_i", "out_i"};
	fct1_names = Passes::Analysis::Instance::get_argument_identifier_names(instance);
	ASSERT_EQ(6u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);
}

TEST(PassesAnalysis_Instance, instance1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Module::ListPtr module_list = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_TRUE(module_list != nullptr);
	ASSERT_EQ(2u, module_list->size());


	AST::Instance::ListPtr instance_list;
	instance_list = Passes::Analysis::Module::get_instance_nodes(module_list->front());
	ASSERT_TRUE(instance_list != nullptr);
	ASSERT_EQ(1u, instance_list->size());

	// First system
	AST::Instance::Ptr instance = instance_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::Instance::get_argument_identifier_nodes(instance);
	ASSERT_EQ(7u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names {"L_INPUT_WIDTH", "L_OUTPUT_WIDTH",
			"clock", "reset", "in0_i", "in1_i", "out_i"};
	fct1_names = Passes::Analysis::Instance::get_argument_identifier_names(instance);
	ASSERT_EQ(7u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);
}

TEST(PassesAnalysis_Instance, instance4) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Module::ListPtr module_list = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_TRUE(module_list != nullptr);
	ASSERT_EQ(3u, module_list->size());

	// Second Module
	module_list->pop_front();

	AST::Instance::ListPtr instance_list;
	instance_list = Passes::Analysis::Module::get_instance_nodes(module_list->front());
	ASSERT_TRUE(instance_list != nullptr);
	ASSERT_EQ(1u, instance_list->size());

	// First system
	AST::Instance::Ptr instance = instance_list->front();

	AST::Identifier::ListPtr fct1_identifiers;
	fct1_identifiers = Passes::Analysis::Instance::get_argument_identifier_nodes(instance);
	ASSERT_EQ(4u, fct1_identifiers->size());

	std::vector<std::string> fct1_names;
	std::vector<std::string> fct1_ref_names {"bits", "q", "d", "clk"};
	fct1_names = Passes::Analysis::Instance::get_argument_identifier_names(instance);
	ASSERT_EQ(4u, fct1_names.size());
	ASSERT_EQ(fct1_ref_names, fct1_names);
}
