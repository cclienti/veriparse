// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <gtest/gtest.h>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>

using namespace Veriparse;

TEST(NodeTest, ConstructorTest)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("NodeTest.ConstructorTest.log");
    Logger::add_stderr_sink();

    AST::WireNet wire1;
    AST::WireNet wire2("test", 5);

    ASSERT_EQ(AST::NodeType::WireNet, wire1.get_node_type());
    ASSERT_TRUE(wire1.is_node_type(AST::NodeType::WireNet));
    ASSERT_TRUE(wire1.is_node_category(AST::NodeType::Node));
    ASSERT_TRUE(wire1.is_node_category(AST::NodeType::Net));
    ASSERT_TRUE(wire1.is_node_category(AST::NodeType::Declaration));
    ASSERT_EQ(std::string(""), wire1.get_filename());
    ASSERT_EQ(0, wire1.get_line());

    ASSERT_EQ(AST::NodeType::WireNet, wire2.get_node_type());
    ASSERT_TRUE(wire2.is_node_type(AST::NodeType::WireNet));
    ASSERT_TRUE(wire2.is_node_category(AST::NodeType::Node));
    ASSERT_TRUE(wire2.is_node_category(AST::NodeType::Net));
    ASSERT_TRUE(wire2.is_node_category(AST::NodeType::Declaration));
    ASSERT_EQ(std::string("test"), wire2.get_filename());
    ASSERT_EQ(5, wire2.get_line());
}

TEST(NodeTest, OperatorEqualOrNot)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("NodeTest.OperatorEqualOrNot.log");
    Logger::add_stderr_sink();

    AST::Var node1;
    node1.set_name("reg");
    AST::Var node2;
    node2.set_name("reg");

    ASSERT_TRUE(node1 == node2);
    ASSERT_TRUE(node1.is_equal(node2));
    ASSERT_FALSE(node1 != node2);
    ASSERT_FALSE(node1.is_not_equal(node2));
}
