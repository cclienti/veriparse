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
	if (!node) {
		return 0;
	}

	if (node->is_node_type(AST::NodeType::IfStatement)) {
		const auto &if_node = AST::cast_to<AST::IfStatement>(node);
		const auto &cond_node = ExpressionEvaluation().evaluate_node(if_node->get_cond());

		if (cond_node) {
			if (cond_node->is_node_type(AST::NodeType::IntConstN)) {
				AST::Node::Ptr stmt;
				if (AST::cast_to<AST::IntConstN>(cond_node)->get_value() != 0) {
					pickup_statements(parent, node, if_node->get_true_statement());
				}
				else {
					pickup_statements(parent, node, if_node->get_false_statement());
				}
				// Recurse in branch selected to find nested statements to
				// process.
				return recurse_in_childs(parent);
			}
		}
	}
	else if (node->is_node_category(AST::NodeType::CaseStatement)) {
		const auto &case_stmt_node = AST::cast_to<AST::CaseStatement>(node);
		auto comp_value = ExpressionEvaluation().evaluate_node(case_stmt_node->get_comp());

		if (comp_value) {
			AST::Node::Ptr selected_case = nullptr;

			for (const auto &cases_node: *case_stmt_node->get_caselist()) {
				const auto &conds = cases_node->get_cond();
				bool stop = false;

				// Default case. It must be used when no normal case
				// matched. So we keep the pointer to the node, and we
				// will analyze it at the end.
				if (!conds) {
					selected_case = cases_node->get_statement();
					continue;
				}

				// Normal case
				for (const auto &cond: *conds) {
					auto cond_value = ExpressionEvaluation().evaluate_node(cond);
					const auto &equal_node = std::make_shared<AST::Eq>(comp_value, cond_value,
					                                                   cases_node->get_filename(), cases_node->get_line());
					auto result = ExpressionEvaluation().evaluate_node(equal_node);
					if (!result) {
						continue;
					}
					if (!result->is_node_type(AST::NodeType::IntConstN)) {
						continue;
					}
					if (AST::cast_to<AST::IntConstN>(result)->get_value()) {
						stop = true;
						break;
					}
				}

				if (stop) {
					selected_case = cases_node->get_statement();
					break;
				}
			}

			if (selected_case) {
				pickup_statements(parent, node, selected_case);
				// Recurse in branch selected to find nested statements to process.
				return recurse_in_childs(parent);
			}
		}
	}

	return recurse_in_childs(node);
}

}
}
}
