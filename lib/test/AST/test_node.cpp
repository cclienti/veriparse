#include <gtest/gtest.h>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>

using namespace Veriparse;

TEST(NodeTest, ConstructorTest) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("NodeTest.ConstructorTest.log");
	Logger::add_stdout_sink();

	AST::Wire wire1;
	AST::Wire wire2("test", 5);

	ASSERT_EQ(AST::NodeType::Wire, wire1.get_node_type());
	ASSERT_TRUE(wire1.is_node_type(AST::NodeType::Wire));
	ASSERT_TRUE(wire1.is_node_category(AST::NodeType::Node));
	ASSERT_TRUE(wire1.is_node_category(AST::NodeType::Variable));
	ASSERT_TRUE(wire1.is_node_category(AST::NodeType::VariableBase));
	ASSERT_EQ(std::string(""), wire1.get_filename());
	ASSERT_EQ(0, wire1.get_line());

	ASSERT_EQ(AST::NodeType::Wire, wire2.get_node_type());
	ASSERT_TRUE(wire2.is_node_type(AST::NodeType::Wire));
	ASSERT_TRUE(wire2.is_node_category(AST::NodeType::Node));
	ASSERT_TRUE(wire2.is_node_category(AST::NodeType::Variable));
	ASSERT_TRUE(wire2.is_node_category(AST::NodeType::VariableBase));
	ASSERT_EQ(std::string("test"), wire2.get_filename());
	ASSERT_EQ(5, wire2.get_line());
}

TEST(NodeTest, OperatorEqualOrNot) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("NodeTest.OperatorEqualOrNot.log");
	Logger::add_stdout_sink();

	AST::Reg node1(nullptr, nullptr, nullptr, false, "reg");
	AST::Reg node2(nullptr, nullptr, nullptr, false, "reg");

	ASSERT_TRUE(node1 == node2);
	ASSERT_TRUE(node1.is_equal(node2));
	ASSERT_FALSE(node1 != node2);
	ASSERT_FALSE(node1.is_not_equal(node2));
}
