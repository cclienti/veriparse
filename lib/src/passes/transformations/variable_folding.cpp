#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/constant_folding.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {


			int VariableFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				if (node) {
					if(node->is_node_type(AST::NodeType::Initial)) {
						m_state_map.clear();
						AST::Initial::Ptr initial = AST::cast_to<AST::Initial>(node);
						return inline_blocking_assignation(initial->get_statement(), node);
					}
				}
				return recurse_in_childs(node);
			}

			int VariableFolding::inline_blocking_assignation(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				switch (node->get_node_type()) {
				case AST::NodeType::Block:
					{
						int ret = 0;
						AST::Node::ListPtr children = node->get_children();
						for (AST::Node::Ptr child: *children) {
							ret += inline_blocking_assignation(child, node);
						}
						return ret;
					}

				case AST::NodeType::BlockingSubstitution:
					{
						AST::BlockingSubstitution::Ptr subst = AST::cast_to<AST::BlockingSubstitution>(node);
						std::string lvalue_str = analyze_lvalue(subst->get_left());
						AST::Node::Ptr const_node = analyze_rvalue(subst->get_right());

						if(lvalue_str.empty() || const_node == nullptr)
							return 0;

						m_state_map[lvalue_str] = const_node;
						subst->get_right()->set_var(const_node);
					}
					break;

				case AST::NodeType::IfStatement:
					{
						AST::IfStatement::Ptr ifstmt = AST::cast_to<AST::IfStatement>(node);
						AST::Node::Ptr expr = ExpressionEvaluation().evaluate_node(ifstmt->get_cond());

						if(expr == nullptr) return 0;

						if(expr->is_node_type(AST::NodeType::IntConstN)) {
							AST::IntConstN::Ptr cond = AST::cast_to<AST::IntConstN>(expr);
							if(cond->get_value() != 0) {
								return inline_blocking_assignation(ifstmt->get_true_statement(), node);
							}
							else {
								return inline_blocking_assignation(ifstmt->get_false_statement(), node);
							}
						}
					}
					break;

				default:
					break;
				}

				return 0;
			}

			AST::Node::Ptr VariableFolding::analyze_rvalue(AST::Rvalue::Ptr rvalue)
			{
				ASTReplace::replace_identifier(AST::to_node(rvalue), m_state_map);
				AST::Node::Ptr expr = ExpressionEvaluation().evaluate_node(rvalue->get_var());
				return expr;
			}

			std::string VariableFolding::analyze_lvalue(AST::Lvalue::Ptr lvalue)
			{
				AST::Node::Ptr var = lvalue->get_var();

				switch(var->get_node_type()) {
				case AST::NodeType::Pointer:
					ASTReplace::replace_identifier(AST::cast_to<AST::Pointer>(var)->get_ptr(), m_state_map);
					break;

				case AST::NodeType::Partselect:
					ASTReplace::replace_identifier(AST::cast_to<AST::Partselect>(var)->get_msb(), m_state_map);
					ASTReplace::replace_identifier(AST::cast_to<AST::Partselect>(var)->get_lsb(), m_state_map);
					break;

				case AST::NodeType::PartselectIndexed:
				case AST::NodeType::PartselectPlusIndexed:
				case AST::NodeType::PartselectMinusIndexed:
					ASTReplace::replace_identifier(AST::cast_to<AST::PartselectIndexed>(var)->get_index(), m_state_map);
					ASTReplace::replace_identifier(AST::cast_to<AST::PartselectIndexed>(var)->get_size(), m_state_map);
					break;

				default:
					break;
				}

				ConstantFolding().run(AST::to_node(lvalue));
				return Generators::VerilogGenerator().render(lvalue->get_var());
			}


		}
	}
}
