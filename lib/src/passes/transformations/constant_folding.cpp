#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ConstantFolding::ConstantFolding(const FunctionMap &function_map) :
	m_function_map (function_map)
{
}

int ConstantFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	if (!node) {
		return 0;
	}

	if (!node->is_node_category(AST::NodeType::Constant)) {
		const auto &expr = ExpressionEvaluation(m_function_map).evaluate_node(node);
		if (expr) {
			parent->replace(node, expr);
		}
	}

	return recurse_in_childs(node);
}


}
}
}
