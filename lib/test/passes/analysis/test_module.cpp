#include "../../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases");
static TestHelpers test2_helpers("lib/test/passes/transformations/testcases");


TEST(PassesAnalysis_Module, ModuleDictionary) {
	ENABLE_LOGGER;

	const auto &node_list = std::make_shared<AST::Node::List>();

	Parser::Verilog verilog;
	verilog.parse(test2_helpers.get_verilog_filename("instance1"));
	const auto &source1 = verilog.get_source();
	ASSERT_TRUE(source1 != nullptr);
	node_list->push_back(source1);

	verilog.parse(test2_helpers.get_verilog_filename("instance2"));
	const auto &source2 = verilog.get_source();
	ASSERT_TRUE(source2 != nullptr);
	node_list->push_back(source2);

	verilog.parse(test2_helpers.get_verilog_filename("instance3"));
	const auto &source3 = verilog.get_source();
	ASSERT_TRUE(source3 != nullptr);
	node_list->push_back(source3);

	Passes::Analysis::Module::ModulesMap str_to_module;
	int ret = Passes::Analysis::Module::get_module_dictionary(node_list, str_to_module);
	ASSERT_EQ(0, ret);
	ASSERT_EQ(7u, str_to_module.size());

	for (const auto &pair: str_to_module) {
		ASSERT_EQ(pair.first, pair.second->get_name());
	}

	ASSERT_EQ(1u, str_to_module.count("instance1"));
	ASSERT_EQ(1u, str_to_module.count("string_test_mod"));
	ASSERT_EQ(1u, str_to_module.count("instance2"));
	ASSERT_EQ(1u, str_to_module.count("my_module2"));
	ASSERT_EQ(1u, str_to_module.count("instance3"));
	ASSERT_EQ(1u, str_to_module.count("my_module3"));
	ASSERT_EQ(1u, str_to_module.count("testbench"));

	node_list->push_back(source3->clone());
	ret = Passes::Analysis::Module::get_module_dictionary(node_list, str_to_module);
	ASSERT_EQ(1, ret);
	ASSERT_EQ(7u, str_to_module.size());

	for (const auto &pair: str_to_module) {
		ASSERT_EQ(pair.first, pair.second->get_name());
	}

	ASSERT_EQ(1u, str_to_module.count("instance1"));
	ASSERT_EQ(1u, str_to_module.count("string_test_mod"));
	ASSERT_EQ(1u, str_to_module.count("instance2"));
	ASSERT_EQ(1u, str_to_module.count("my_module2"));
	ASSERT_EQ(1u, str_to_module.count("instance3"));
	ASSERT_EQ(1u, str_to_module.count("my_module3"));
	ASSERT_EQ(1u, str_to_module.count("testbench"));
}

TEST(PassesAnalysis_Module, parameter0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Parameter::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_parameter_nodes(source);
	ASSERT_EQ(2u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"WIDTH", "RESET_VALUE"};
	names = Passes::Analysis::Module::get_parameter_names(source);
	ASSERT_EQ(2u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, localparam0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Localparam::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_localparam_nodes(source);
	ASSERT_EQ(3u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"DOUBLE_WIDTH", "MYLOCALPARAM1", "MYLOCALPARAM2"};
	names = Passes::Analysis::Module::get_localparam_names(source);
	ASSERT_EQ(3u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, parameter1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Parameter::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_parameter_nodes(source);
	ASSERT_EQ(2u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"WIDTH", "RESET_VALUE"};
	names = Passes::Analysis::Module::get_parameter_names(source);
	ASSERT_EQ(2u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, function0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Function::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_function_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_function_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, function1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Function::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_function_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_function_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, function2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Function::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_function_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"my_func"};
	names = Passes::Analysis::Module::get_function_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, function3) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Function::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_function_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_function_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, expression42) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::FunctionCall::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_functioncall_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"myfunc"};
	names = Passes::Analysis::Module::get_functioncall_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, expression43) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::SystemCall::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_systemcall_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"system"};
	names = Passes::Analysis::Module::get_systemcall_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, width1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::TaskCall::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_taskcall_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"word"};
	names = Passes::Analysis::Module::get_taskcall_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, task0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Task::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_task_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_task_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, task1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Task::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_task_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_task_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, task2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Task::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_task_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names{"clogb2"};
	names = Passes::Analysis::Module::get_task_names(source);
	ASSERT_EQ(1u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, expression37) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Lvalue::ListPtr lvalue_nodes;
	lvalue_nodes = Passes::Analysis::Module::get_lvalue_nodes(source);
	ASSERT_EQ(1u, lvalue_nodes->size());

	AST::Rvalue::ListPtr rvalue_nodes;
	rvalue_nodes = Passes::Analysis::Module::get_rvalue_nodes(source);
	ASSERT_EQ(2u, rvalue_nodes->size());

	AST::Identifier::ListPtr rvalue_identifier_nodes;
	rvalue_identifier_nodes = Passes::Analysis::Module::get_rvalue_identifier_nodes(source);
	ASSERT_EQ(1u, rvalue_identifier_nodes->size());

	std::vector<std::string> rvalue_identifier_names;
	std::vector<std::string> ref_rvalue_identifier_names{"in"};
	rvalue_identifier_names = Passes::Analysis::Module::get_rvalue_identifier_names(source);
	ASSERT_EQ(1u, rvalue_identifier_names.size());
	ASSERT_EQ(ref_rvalue_identifier_names, rvalue_identifier_names);
}

TEST(PassesAnalysis_Module, module0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);
	test_helpers.render_node_to_dot_file(source, test_string + ".dot");

	AST::Node::ListPtr ports;
	ports = Passes::Analysis::Module::get_port_nodes(source);
	ASSERT_EQ(5u, ports->size());
	std::vector<std::string> ref_port_names{"reset", "clk", "in0", "in1", "out"};
	std::vector<std::string> port_names = Passes::Analysis::Module::get_port_names(source);
	ASSERT_EQ(5u, port_names.size());
	ASSERT_EQ(ref_port_names, port_names);

	AST::IODir::ListPtr iodir_nodes;
	iodir_nodes = Passes::Analysis::Module::get_iodir_nodes(source);
	ASSERT_EQ(5u, iodir_nodes->size());
	std::vector<std::string> iodir_names;
	std::vector<std::string> ref_iodir_names{"reset", "clk", "in0", "in1", "out"};
	iodir_names = Passes::Analysis::Module::get_iodir_names(source);
	ASSERT_EQ(5u, iodir_names.size());
	ASSERT_EQ(ref_iodir_names, iodir_names);

	AST::Output::ListPtr output_nodes;
	output_nodes = Passes::Analysis::Module::get_output_nodes(source);
	ASSERT_EQ(1u, output_nodes->size());
	std::vector<std::string> output_names;
	std::vector<std::string> ref_output_names{"out"};
	output_names = Passes::Analysis::Module::get_output_names(source);
	ASSERT_EQ(1u, output_names.size());
	ASSERT_EQ(ref_output_names, output_names);

	AST::Input::ListPtr input_nodes;
	input_nodes = Passes::Analysis::Module::get_input_nodes(source);
	ASSERT_EQ(4u, input_nodes->size());
	std::vector<std::string> input_names;
	std::vector<std::string> ref_input_names{"reset", "clk", "in0", "in1"};
	input_names = Passes::Analysis::Module::get_input_names(source);
	ASSERT_EQ(4u, input_names.size());
	ASSERT_EQ(ref_input_names, input_names);
}

TEST(PassesAnalysis_Module, bidir0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Node::ListPtr ports;
	ports = Passes::Analysis::Module::get_port_nodes(source);
	ASSERT_EQ(5u, ports->size());
	std::vector<std::string> ref_port_names{"oe", "clk", "inp", "outp", "bidir"};
	std::vector<std::string> port_names = Passes::Analysis::Module::get_port_names(source);
	ASSERT_EQ(5u, port_names.size());
	ASSERT_EQ(ref_port_names, port_names);

	AST::IODir::ListPtr iodir_nodes;
	iodir_nodes = Passes::Analysis::Module::get_iodir_nodes(source);
	ASSERT_EQ(5u, iodir_nodes->size());
	std::vector<std::string> iodir_names;
	std::vector<std::string> ref_iodir_names{"oe", "clk", "inp", "outp", "bidir"};
	iodir_names = Passes::Analysis::Module::get_iodir_names(source);
	ASSERT_EQ(5u, iodir_names.size());
	ASSERT_EQ(ref_iodir_names, iodir_names);

	AST::Output::ListPtr output_nodes;
	output_nodes = Passes::Analysis::Module::get_output_nodes(source);
	ASSERT_EQ(1u, output_nodes->size());
	std::vector<std::string> output_names;
	std::vector<std::string> ref_output_names{"outp"};
	output_names = Passes::Analysis::Module::get_output_names(source);
	ASSERT_EQ(1u, output_names.size());
	ASSERT_EQ(ref_output_names, output_names);

	AST::Inout::ListPtr inout_nodes;
	inout_nodes = Passes::Analysis::Module::get_inout_nodes(source);
	ASSERT_EQ(1u, inout_nodes->size());
	std::vector<std::string> inout_names;
	std::vector<std::string> ref_inout_names{"bidir"};
	inout_names = Passes::Analysis::Module::get_inout_names(source);
	ASSERT_EQ(1u, inout_names.size());
	ASSERT_EQ(ref_inout_names, inout_names);

	AST::Input::ListPtr input_nodes;
	input_nodes = Passes::Analysis::Module::get_input_nodes(source);
	ASSERT_EQ(3u, input_nodes->size());
	std::vector<std::string> input_names;
	std::vector<std::string> ref_input_names{"oe", "clk", "inp"};
	input_names = Passes::Analysis::Module::get_input_names(source);
	ASSERT_EQ(3u, input_names.size());
	ASSERT_EQ(ref_input_names, input_names);
}

TEST(PassesAnalysis_Module, module1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Node::ListPtr ports;
	ports = Passes::Analysis::Module::get_port_nodes(source);
	ASSERT_EQ(5u, ports->size());
	std::vector<std::string> ref_port_names{"reset", "clk", "in0", "in1", "out"};
	std::vector<std::string> port_names = Passes::Analysis::Module::get_port_names(source);
	ASSERT_EQ(5u, port_names.size());
	ASSERT_EQ(ref_port_names, port_names);

	AST::IODir::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_iodir_nodes(source);
	ASSERT_EQ(5u, nodes->size());
	std::vector<std::string> names;
	std::vector<std::string> ref_names{"reset", "clk", "in0", "in1", "out"};
	names = Passes::Analysis::Module::get_iodir_names(source);
	ASSERT_EQ(5u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, module2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Node::ListPtr ports;
	ports = Passes::Analysis::Module::get_port_nodes(source);
	ASSERT_EQ(7u, ports->size());
	std::vector<std::string> ref_port_names{"reset", "clk", "in0", "in1", "in2", "in3", "out"};
	std::vector<std::string> port_names = Passes::Analysis::Module::get_port_names(source);
	ASSERT_EQ(7u, port_names.size());
	ASSERT_EQ(ref_port_names, port_names);

	AST::IODir::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_iodir_nodes(source);
	ASSERT_EQ(7u, nodes->size());
	std::vector<std::string> names;
	std::vector<std::string> ref_names{"reset", "clk", "in0", "in1", "in2", "in3", "out"};
	names = Passes::Analysis::Module::get_iodir_names(source);
	ASSERT_EQ(7u, names.size());
	ASSERT_EQ(ref_names, names);
}

TEST(PassesAnalysis_Module, net0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(12u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "x", "c", "h", "d", "e", "f"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(12u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "x", "c", "h", "d", "e", "f"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(8u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(8u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("x", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("h", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("d", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("e", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("f", AST::cast_to<AST::Wire>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, net1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(7u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(7u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(3u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(3u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Tri>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Tri>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Tri>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, net2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(7u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(7u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(3u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(3u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Supply0>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Supply0>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Supply0>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, net3) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(7u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(7u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(3u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(3u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Supply1>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Supply1>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Supply1>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, var0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(11u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c", "h", "d", "e", "f"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(11u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c", "h", "d", "e", "f"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(7u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(7u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("h", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("d", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("e", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("f", AST::cast_to<AST::Reg>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, var1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(11u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c", "h", "d", "e", "f"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(11u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c", "h", "d", "e", "f"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(7u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(7u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("h", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("d", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("e", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("f", AST::cast_to<AST::Integer>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, var2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Variable::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_variable_nodes(source);
	ASSERT_EQ(11u, nodes->size());

	std::vector<std::string> names;
	std::vector<std::string> ref_names = {"reset", "clk", "in", "out",
	                                      "a", "b", "c", "h", "d", "e", "f"};
	names = Passes::Analysis::Module::get_variable_names(source);
	ASSERT_EQ(11u, names.size());
	ASSERT_EQ(ref_names, names);

	std::vector<std::string> net_only_names;
	std::vector<std::string> net_only_ref_names = {"a", "b", "c", "h", "d", "e", "f"};
	net_only_names = Passes::Analysis::Module::get_variable_names_within_module(source);
	ASSERT_EQ(7u, net_only_names.size());
	ASSERT_EQ(net_only_ref_names, net_only_names);

	AST::Variable::ListPtr net_only_nodes;
	net_only_nodes = Passes::Analysis::Module::get_variable_nodes_within_module(source);
	ASSERT_EQ(7u, net_only_nodes->size());
	ASSERT_EQ("a", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("b", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("c", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("h", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("d", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("e", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
	ASSERT_EQ("f", AST::cast_to<AST::Real>(net_only_nodes->front())->get_name()); net_only_nodes->pop_front();
}

TEST(PassesAnalysis_Module, instance0) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Instance::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_instance_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	AST::Module::ListPtr module_nodes;
	module_nodes = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	std::vector<std::string> module_names;
	std::vector<std::string> ref_module_names{"instance0", "mod"};
	module_names = Passes::Analysis::Module::get_module_names(source);
	ASSERT_EQ(2u, module_names.size());
	ASSERT_EQ(ref_module_names, module_names);

	AST::Lvalue::ListPtr lvalue_nodes;
	AST::Module::Ptr instance0 = module_nodes->front();
	module_nodes->pop_front();
	AST::Module::Ptr mod = module_nodes->front();
	module_nodes->pop_front();
	lvalue_nodes = Passes::Analysis::Module::get_lvalue_nodes(mod);
	ASSERT_EQ(2u, lvalue_nodes->size());
}

TEST(PassesAnalysis_Module, instance1) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Instance::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_instance_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	AST::Module::ListPtr module_nodes;
	module_nodes = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	std::vector<std::string> module_names;
	std::vector<std::string> ref_module_names{"instance1", "mod"};
	module_names = Passes::Analysis::Module::get_module_names(source);
	ASSERT_EQ(2u, module_names.size());
	ASSERT_EQ(ref_module_names, module_names);

	AST::Lvalue::ListPtr lvalue_nodes;
	AST::Module::Ptr instance0 = module_nodes->front();
	module_nodes->pop_front();
	AST::Module::Ptr mod = module_nodes->front();
	module_nodes->pop_front();
	lvalue_nodes = Passes::Analysis::Module::get_lvalue_nodes(mod);
	ASSERT_EQ(4u, lvalue_nodes->size());
}

TEST(PassesAnalysis_Module, instance2) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Instance::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_instance_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	AST::Module::ListPtr module_nodes;
	module_nodes = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	std::vector<std::string> module_names;
	std::vector<std::string> ref_module_names{"instance2", "mod"};
	module_names = Passes::Analysis::Module::get_module_names(source);
	ASSERT_EQ(2u, module_names.size());
	ASSERT_EQ(ref_module_names, module_names);
}

TEST(PassesAnalysis_Module, instance3) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Instance::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_instance_nodes(source);
	ASSERT_EQ(1u, nodes->size());

	AST::Module::ListPtr module_nodes;
	module_nodes = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(2u, module_nodes->size());

	std::vector<std::string> module_names;
	std::vector<std::string> ref_module_names{"my_module", "top"};
	module_names = Passes::Analysis::Module::get_module_names(source);
	ASSERT_EQ(2u, module_names.size());
	ASSERT_EQ(ref_module_names, module_names);
}

TEST(PassesAnalysis_Module, instance4) {
	ENABLE_LOGGER;

	Parser::Verilog verilog;
	verilog.parse(test_helpers.get_verilog_filename(test_name));
	AST::Node::Ptr source = verilog.get_source();
	ASSERT_TRUE(source != nullptr);

	AST::Instance::ListPtr nodes;
	nodes = Passes::Analysis::Module::get_instance_nodes(source);
	ASSERT_EQ(3u, nodes->size());

	AST::Module::ListPtr module_nodes;
	module_nodes = Passes::Analysis::Module::get_module_nodes(source);
	ASSERT_EQ(3u, module_nodes->size());

	std::vector<std::string> module_names;
	std::vector<std::string> ref_module_names{"DFF", "dffn", "MxN_pipeline"};
	module_names = Passes::Analysis::Module::get_module_names(source);
	ASSERT_EQ(3u, module_names.size());
	ASSERT_EQ(ref_module_names, module_names);
}
