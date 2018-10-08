#include <veriparse/passes/transformations/generate_removal.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse
{
namespace Passes
{
namespace Transformations
{


int GenerateRemoval::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if (!node) {
		return 0;
	}

	switch (node->get_node_type())
	{
	case AST::NodeType::Genvar:
		{
		}
		break;

	case AST::NodeType::GenerateStatement:
		{
			if (is_generate_removable(node)) {
				const auto &gen_stmt = AST::cast_to<AST::GenerateStatement>(node);
				pickup_statements(parent, node, gen_stmt->get_items());
			}
		}
		break;

	default:
		return recurse_in_childs(node);
	}

	return 0;
}

bool GenerateRemoval::is_generate_removable(const AST::Node::Ptr &node)
{
	if (!node) {
		return true;
	}

	switch (node->get_node_type())
	{
	case AST::NodeType::Initial:
	case AST::NodeType::Always:
	case AST::NodeType::Task:
	case AST::NodeType::Function:
		break;

	case AST::NodeType::IfStatement:
	case AST::NodeType::ForStatement:
	case AST::NodeType::WhileStatement:
	case AST::NodeType::RepeatStatement:
	case AST::NodeType::CaseStatement:
	case AST::NodeType::CasexStatement:
	case AST::NodeType::CasezStatement:
		return false;

	default:
		bool ret = true;
		const auto &children = node->get_children();
		for (const auto &child: *children) {
			ret &= is_generate_removable(child);
		}
		return ret;
	}

	return true;
}


}
}
}
