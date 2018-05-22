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
									stmt = if_node->get_true_statement();
								}
								else {
									stmt = if_node->get_false_statement();
								}

								AST::Node::ListPtr stmts;
								if(stmt->is_node_type(AST::NodeType::Block)) {
									stmts = AST::cast_to<AST::Block>(stmt)->get_statements();
								}
								else {
									stmts = std::make_shared<AST::Node::List>();
									stmts->push_back(stmt);
								}

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
										parent = block;
									}
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
