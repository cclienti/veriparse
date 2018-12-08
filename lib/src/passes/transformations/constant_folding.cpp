#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


int ConstantFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent) {
	if (node) {

		if(!node->is_node_category(AST::NodeType::Constant)) {
			AST::Node::Ptr expr = ExpressionEvaluation().evaluate_node(node);
			if(expr) {
				parent->replace(node, expr);
			}
		}
	}

	return recurse_in_childs(node);
}


}
}
}
