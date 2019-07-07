#include <gtest/gtest.h>
#include <veriparse/misc/tree.hpp>
#include <veriparse/logger/logger.hpp>

#include <iostream>


using namespace Veriparse;


TEST(MiscTest, Tree_node_0)
{
	using TNode = Misc::TreeNode<std::string>;

	auto node0 = std::make_unique<TNode>("inst0");
	auto node1 = std::make_unique<TNode>("inst1");
	auto node2 = std::make_unique<TNode>("inst2");
	auto node3 = std::make_unique<TNode>("inst3");
	auto node4 = std::make_unique<TNode>("inst4");

	node2->push_child(std::move(node3));
	node2->push_child(std::move(node4));
	node1->push_child(std::move(node2));
	node0->push_child(std::move(node1));

	std::string ref {"digraph G {\n"
	                 "\trankdir=LR;\n"
	                 "	n0 [label=inst0];\n"
	                 "	n0 -> n1\n"
	                 "	n1 [label=inst1];\n"
	                 "	n1 -> n2\n"
	                 "	n2 [label=inst2];\n"
	                 "	n2 -> n3\n"
	                 "	n3 [label=inst3];\n"
	                 "	n2 -> n4\n"
	                 "	n4 [label=inst4];\n"
	                 "}\n"};

	ASSERT_EQ(node0->to_dot(), ref);
}