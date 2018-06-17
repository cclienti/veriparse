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
						return execute(initial->get_statement(), node);
					}
				}
				return recurse_in_childs(node);
			}

			int VariableFolding::execute(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				switch (node->get_node_type()) {
				case AST::NodeType::Block:
				case AST::NodeType::SingleStatement:
						return execute_in_childs(node);

				case AST::NodeType::BlockingSubstitution:
					return execute_blocking_substitution(AST::cast_to<AST::BlockingSubstitution>(node), parent);

				case AST::NodeType::IfStatement:
					return execute_if(AST::cast_to<AST::IfStatement>(node), parent);

				case AST::NodeType::ForStatement:
					return execute_for(AST::cast_to<AST::ForStatement>(node), parent);

				case AST::NodeType::WhileStatement:
					return execute_while(AST::cast_to<AST::WhileStatement>(node), parent);

				case AST::NodeType::RepeatStatement:
					return execute_repeat(AST::cast_to<AST::RepeatStatement>(node), parent);

				case AST::NodeType::FunctionCall:
				case AST::NodeType::TaskCall:
				case AST::NodeType::SystemCall:
					return execute_call(node, parent);
				default:
					break;
				}

				return 0;
			}

			int VariableFolding::execute_in_childs(AST::Node::Ptr node)
			{
				int ret = 0;
				AST::Node::ListPtr children = node->get_children();
				for (AST::Node::Ptr child: *children) {
					ret += execute(child, node);
				}
				return ret;
			}

			int VariableFolding::execute_blocking_substitution(AST::BlockingSubstitution::Ptr subst, AST::Node::Ptr parent)
			{
				std::string lvalue_str = analyze_lvalue(subst->get_left());
				AST::Node::Ptr const_node = analyze_rvalue(subst->get_right());

				if(lvalue_str.empty() || const_node == nullptr)
					return 0;

				m_state_map[lvalue_str] = const_node;
				subst->get_right()->set_var(const_node);
				return 0;
			}

			int VariableFolding::execute_if(AST::IfStatement::Ptr ifstmt, AST::Node::Ptr parent)
			{

				auto expr = analyze_expression(ifstmt->get_cond());

				if(expr) {
					if(expr->is_node_type(AST::NodeType::IntConstN)) {
						ifstmt->set_cond(expr);
						auto cond = AST::cast_to<AST::IntConstN>(expr);
						AST::Node::Ptr selected_stmt;

						if(cond->get_value() != 0) {
							selected_stmt = ifstmt->get_true_statement();
						}
						else {
							selected_stmt = ifstmt->get_false_statement();
						}

						return execute(selected_stmt, ifstmt);
					}
				}

				return 0;
			}

			int VariableFolding::execute_for(AST::ForStatement::Ptr node, AST::Node::Ptr parent)
			{
				return 0;
			}

			int VariableFolding::execute_while(AST::WhileStatement::Ptr node, AST::Node::Ptr parent)
			{
				return 0;
			}

			int VariableFolding::execute_repeat(AST::RepeatStatement::Ptr node, AST::Node::Ptr parent)
			{
				int ret = 0;

				auto expr = analyze_expression(node->get_times());

				if(expr) {
					if(expr->is_node_type(AST::NodeType::IntConstN)) {
						// Create a temporary block to hold stmts.
						auto block = std::make_shared<AST::Block>();
						auto block_stmts = std::make_shared<AST::Node::List>();
						block->set_statements(block_stmts);

						auto times = AST::cast_to<AST::IntConstN>(expr);
						for(mpz_class i=0; i<times->get_value(); i++) {
							auto current = node->get_statement()->clone();

							if(current->is_node_type(AST::NodeType::Block)) {
								auto current_block = AST::cast_to<AST::Block>(current);
								for(auto stmt: *current_block->get_statements()) {
									block_stmts->push_back(stmt);
								}
							}
							else {
								block_stmts->push_back(current);
							}
						}

						pickup_statements(parent, node, block);

						// We must call execute on each element of the
						// temporary block to pass the right parentt.
						for(auto stmt: *block_stmts) {
							ret += execute(stmt, parent);
						}
					}
				}

				return ret;
			}

			int VariableFolding::execute_call(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				AST::Node::ListPtr args;

				switch(node->get_node_type()) {
				case AST::NodeType::FunctionCall:
					args = AST::cast_to<AST::FunctionCall>(node)->get_args();
					break;
				case AST::NodeType::TaskCall:
					args = AST::cast_to<AST::TaskCall>(node)->get_args();
					break;
				case AST::NodeType::SystemCall:
					args = AST::cast_to<AST::SystemCall>(node)->get_args();
					break;
				default:
					LOG_ERROR_N(node) << "VariableFolding: unknown node type";
					return 1;
				}

				if(args) {
					for(auto arg: *args) {
						ASTReplace::replace_identifier(arg, m_state_map, node);
						auto expr = ExpressionEvaluation().evaluate_node(arg);
						if(expr) {
							node->replace(arg, expr);
						}
					}
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

			AST::Node::Ptr VariableFolding::analyze_expression(AST::Node::Ptr expr)
			{
				auto rvalue = std::make_shared<AST::Rvalue>(expr);
				return analyze_rvalue(rvalue);
			}

		}
	}
}
