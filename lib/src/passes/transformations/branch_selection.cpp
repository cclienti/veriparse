#include <veriparse/passes/transformations/branch_selection.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse
{
	namespace Passes
	{
		namespace Transformations
		{

			int BranchSelection::process(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				if (node) {
					if(node->is_node_type(AST::NodeType::IfStatement)) {

						const AST::IfStatement::Ptr if_node = AST::cast_to<AST::IfStatement>(node);
						AST::Node::Ptr cond_node = ExpressionEvaluation().evaluate_node(if_node->get_cond());

						if(cond_node) {
							if(cond_node->is_node_type(AST::NodeType::IntConstN)) {
								AST::Node::Ptr stmt;
								if(AST::cast_to<AST::IntConstN>(cond_node)->get_value() != 0) {
									pickup_statements(parent, node, if_node->get_true_statement());
								}
								else {
									pickup_statements(parent, node, if_node->get_false_statement());
								}
							}
						}

					}
				}

				return recurse_in_childs(node);
			}

		}
	}
}
