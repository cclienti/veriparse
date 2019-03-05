#include <veriparse/passes/transformations/transformation_base.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


int TransformationBase::recurse(AST::Node::Ptr parent, AST::Node::ListPtr node_list, ProcessFunction function)
{
	int ret = 0;
	if(node_list) {
		for (const auto &node: *node_list) {
			ret += function(node, parent);
		}
	}
	return ret;
}


int TransformationBase::recurse_in_childs(AST::Node::Ptr node)
{
	ProcessFunction process_function =
		[this](AST::Node::Ptr node, AST::Node::Ptr parent) -> int {return this->process(node, parent);};

	return recurse_in_childs(node, process_function);
}


int TransformationBase::recurse_in_childs(AST::Node::Ptr node, ProcessFunction function)
{
	int ret = 0;
	if(node) {
		recurse(node, node->get_children(), function);
		// AST::Node::ListPtr children = node->get_children();
		// for (AST::Node::Ptr child: *children) {
		// 	ret += function(child, node);
		// }
	}
	return ret;
}


void TransformationBase::pickup_statements(AST::Node::Ptr parent, AST::Node::Ptr node,
                                           AST::Node::ListPtr stmts)
{
	if(stmts) {
		// Replace the unrolled statements in the parent block
		if (parent->is_node_type(AST::NodeType::Block)) {
			parent->replace(node, stmts);
		}
		else {
			// if the node to replace is already in a list,
			// we can directly replace the node by our
			// list. There is no need to create a block.
			bool node_in_list = parent->replace(node, stmts);

			// We create a block to store our list.
			if (!node_in_list) {
				AST::Block::Ptr block = std::make_shared<AST::Block>(stmts, "");
				parent->replace(node, block);
			}
		}
	}
	else {
		parent->replace(node, AST::Node::Ptr(nullptr));
	}

}

void TransformationBase::pickup_statements(AST::Node::Ptr parent, AST::Node::Ptr node,
                                           AST::Node::Ptr stmt)
{
	if(stmt) {
		AST::Node::ListPtr stmts;
		if(stmt->is_node_type(AST::NodeType::Block)) {
			stmts = AST::cast_to<AST::Block>(stmt)->get_statements();
		}
		else {
			stmts = std::make_shared<AST::Node::List>();
			stmts->push_back(stmt);
		}

		return pickup_statements(parent, node, stmts);
	}
	else {
		parent->replace(node, stmt);
	}
}


}
}
}
