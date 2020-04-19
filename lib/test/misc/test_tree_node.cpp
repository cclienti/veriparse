#include <gtest/gtest.h>
#include <veriparse/misc/tree.hpp>
#include <veriparse/logger/logger.hpp>

#include <iostream>


using namespace Veriparse;


class TNode: public Misc::TreeNode<std::string>
{
public:
	TNode(const std::string &value): TreeNode(value) {}
	virtual ~TNode() {}

	static void reset() {
		TNode::set_tree_node_id(0);
	}

private:
	std::string print_value() const final
	{
		return get_value();
	}

	Ptr make_ptr(const std::string &value) const final
	{
		return std::make_unique<TNode>(value);
	}
};


TEST(MiscTest, Tree_node_0)
{
	TNode::reset();

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
	                 "	n0 [label=\"inst0\"];\n"
	                 "	n0 -> n1\n"
	                 "	n1 [label=\"inst1\"];\n"
	                 "	n1 -> n2\n"
	                 "	n2 [label=\"inst2\"];\n"
	                 "	n2 -> n3\n"
	                 "	n3 [label=\"inst3\"];\n"
	                 "	n2 -> n4\n"
	                 "	n4 [label=\"inst4\"];\n"
	                 "}\n"};

	ASSERT_EQ(node0->to_dot(), ref);
}

TEST(MiscTest, Tree_node_0_clone)
{
	TNode::reset();

	auto node0 = std::make_unique<TNode>("inst0");
	auto node1 = std::make_unique<TNode>("inst1");
	auto node2 = std::make_unique<TNode>("inst2");
	auto node3 = std::make_unique<TNode>("inst3");
	auto node4 = std::make_unique<TNode>("inst4");

	node2->push_child(std::move(node3));
	node2->push_child(std::move(node4));
	node1->push_child(std::move(node2));
	node0->push_child(std::move(node1));

	auto node0_clone = node0->clone();

	std::string ref {"digraph G {\n"
	                 "\trankdir=LR;\n"
	                 "	n0 [label=\"inst0\"];\n"
	                 "	n0 -> n1\n"
	                 "	n1 [label=\"inst1\"];\n"
	                 "	n1 -> n2\n"
	                 "	n2 [label=\"inst2\"];\n"
	                 "	n2 -> n3\n"
	                 "	n3 [label=\"inst3\"];\n"
	                 "	n2 -> n4\n"
	                 "	n4 [label=\"inst4\"];\n"
	                 "}\n"};

	ASSERT_EQ(node0_clone->to_dot(), ref);
}
