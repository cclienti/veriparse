#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/transformations/expression_operators.hpp>
#include <veriparse/passes/transformations/expression_operators_dispatcher.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>
#include <functional>




namespace Veriparse {
	namespace Passes {
		namespace Transformations {
			ExpressionEvaluation::ExpressionEvaluation()	{}

			ExpressionEvaluation::ExpressionEvaluation(const replace_map_t &replace_identifier):
				m_replace_identifier(replace_identifier) {}


			AST::Node::Ptr ExpressionEvaluation::evaluate_node(const AST::Node::Ptr node)	{
				if (node) {
					if (node->is_node_type(AST::NodeType::Cond)) {
						AST::Cond::Ptr cond_node = AST::cast_to<AST::Cond>(node);
						AST::Node::Ptr cond_value = evaluate_node(cond_node->get_cond());
						AST::Node::Ptr cond_true = evaluate_node(cond_node->get_left());
						AST::Node::Ptr cond_false = evaluate_node(cond_node->get_right());
						return evaluate_constant_node(AST::NodeType::Cond, cond_value, cond_true, cond_false);
					}
					else if (node->is_node_category(AST::NodeType::UnaryOperator)) {
						AST::UnaryOperator::Ptr unary_node = AST::cast_to<AST::UnaryOperator>(node);
						AST::Node::Ptr right = evaluate_node(unary_node->get_right());
						return evaluate_constant_node(unary_node->get_node_type(), right);
					}
					else if (node->is_node_category(AST::NodeType::Operator)) {
						AST::Operator::Ptr binary_node = AST::cast_to<AST::Operator>(node);
						AST::Node::Ptr left = evaluate_node(binary_node->get_left());
						AST::Node::Ptr right = evaluate_node(binary_node->get_right());
						return evaluate_constant_node(binary_node->get_node_type(), left, right);
					}
					else if (node->is_node_type(AST::NodeType::Pointer)) {
						AST::Pointer::Ptr pointer_node = AST::cast_to<AST::Pointer>(node);
						AST::IntConstN::Ptr var = AST::cast_to<AST::IntConstN>(evaluate_node(pointer_node->get_var()));
						AST::IntConstN::Ptr ptr = AST::cast_to<AST::IntConstN>(evaluate_node(pointer_node->get_ptr()));
						pointer<AST::IntConstN::Ptr> op;
						return AST::to_node(op(var, ptr));
					}
					else if (node->is_node_type(AST::NodeType::Partselect)) {
						AST::Partselect::Ptr partselect_node = AST::cast_to<AST::Partselect>(node);
						AST::IntConstN::Ptr var = AST::cast_to<AST::IntConstN>(evaluate_node(partselect_node->get_var()));
						AST::IntConstN::Ptr msb = AST::cast_to<AST::IntConstN>(evaluate_node(partselect_node->get_msb()));
						AST::IntConstN::Ptr lsb = AST::cast_to<AST::IntConstN>(evaluate_node(partselect_node->get_lsb()));
						partselect<AST::IntConstN::Ptr> op;
						return AST::to_node(op(var, msb, lsb));
					}
					else if (node->is_node_category(AST::NodeType::PartselectIndexed)) {
						AST::PartselectIndexed::Ptr partselect_node = AST::cast_to<AST::PartselectIndexed>(node);
						AST::Node::Ptr var = evaluate_node(partselect_node->get_var());
						AST::Node::Ptr index = evaluate_node(partselect_node->get_index());
						AST::Node::Ptr size = evaluate_node(partselect_node->get_size());
						return evaluate_constant_node(node->get_node_type(), var, index, size);
					}
					else if (node->is_node_type(AST::NodeType::SystemCall)) {
						AST::SystemCall::Ptr syscall_node = AST::cast_to<AST::SystemCall>(node);
						AST::Node::ListPtr syscall_args = syscall_node->get_args();
						AST::Node::ListPtr args = std::make_shared<AST::Node::List>();
						for (AST::Node::Ptr syscall_arg: *syscall_args) {
							args->push_back(evaluate_node(syscall_arg));
						}
						return evaluate_system_call(syscall_node->get_syscall(), args,
						                            syscall_node->get_filename(), syscall_node->get_line());
					}
					else if (node->is_node_type(AST::NodeType::IntConstN)) {
						return node->clone();
					}
					else if (node->is_node_type(AST::NodeType::FloatConst)) {
						return node->clone();
					}
					else if (node->is_node_type(AST::NodeType::Identifier)) {
						std::string value = AST::cast_to<AST::Identifier>(node)->get_name();
						replace_map_t::const_iterator search = m_replace_identifier.find(value);
						if(search != m_replace_identifier.cend()) {
							return search->second->clone();
						}
					}
				}
				return nullptr;
			}

			AST::Node::Ptr ExpressionEvaluation::evaluate_system_call(const std::string &call,
			                                                          const AST::Node::ListPtr args,
			                                                          const std::string &filename, uint32_t line) {
				if(args) {
					if(args->size() == 1) {
						if (call == "clog2") {
							return expression_operator_unary<clog2>(args->front());
						}
						else if (call == "unsigned") {
							return expression_operator_unary<unsigned_fct>(args->front());
						}
						else if (call == "signed") {
							return expression_operator_unary<signed_fct>(args->front());
						}
					}
					else {
						LOG_ERROR << filename << ":" << line << ": too much arguments in " << call;
					}
				}

				return nullptr;
			}

			AST::Node::Ptr ExpressionEvaluation::evaluate_constant_node(const AST::NodeType operation,
			                                                            const AST::Node::Ptr node0) {
				switch(operation) {
				case AST::NodeType::Uplus:  return expression_operator_unary<uplus>(node0);
				case AST::NodeType::Uminus: return expression_operator_unary<uminus>(node0);
				case AST::NodeType::Ulnot:  return expression_operator_unary_nofloat<ulnot>(node0);
				case AST::NodeType::Unot:   return expression_operator_unary_nofloat<unot>(node0);
				case AST::NodeType::Uand:   return expression_operator_unary_nofloat<uand>(node0);
				case AST::NodeType::Unand:  return expression_operator_unary_nofloat<unand>(node0);
				case AST::NodeType::Uor:    return expression_operator_unary_nofloat<uor>(node0);
				case AST::NodeType::Unor:   return expression_operator_unary_nofloat<unor>(node0);
				case AST::NodeType::Uxor:   return expression_operator_unary_nofloat<uxor>(node0);
				case AST::NodeType::Uxnor:  return expression_operator_unary_nofloat<uxnor>(node0);
				default:
					return nullptr;
				}
			}

			AST::Node::Ptr ExpressionEvaluation::evaluate_constant_node(const AST::NodeType operation,
			                                                            const AST::Node::Ptr node0,
			                                                            const AST::Node::Ptr node1) {
				switch(operation) {
				case AST::NodeType::Power:       return expression_operator_binary<power>(node0, node1);
				case AST::NodeType::Times:       return expression_operator_binary<times>(node0, node1);
				case AST::NodeType::Divide:      return expression_operator_binary<divide>(node0, node1);
				case AST::NodeType::Mod:         return expression_operator_binary<mod>(node0, node1);
				case AST::NodeType::Plus:        return expression_operator_binary<plus>(node0, node1);
				case AST::NodeType::Minus:       return expression_operator_binary<minus>(node0, node1);
				case AST::NodeType::Sll:         return expression_operator_binary_nofloat<sll>(node0, node1);
				case AST::NodeType::Srl:         return expression_operator_binary_nofloat<sll>(node0, node1);
				case AST::NodeType::Sra:         return expression_operator_binary_nofloat<sra>(node0, node1);
				case AST::NodeType::LessThan:    return expression_operator_binary<less_than>(node0, node1);
				case AST::NodeType::GreaterThan: return expression_operator_binary<greater_than>(node0, node1);
				case AST::NodeType::LessEq:      return expression_operator_binary<less_equal>(node0, node1);
				case AST::NodeType::GreaterEq:   return expression_operator_binary<greater_equal>(node0, node1);
				case AST::NodeType::Eq:          return expression_operator_binary<equal>(node0, node1);
				case AST::NodeType::NotEq:       return expression_operator_binary<not_equal>(node0, node1);
				case AST::NodeType::Eql:         return expression_operator_binary_nofloat<case_equal>(node0, node1);
				case AST::NodeType::NotEql:      return expression_operator_binary_nofloat<case_not_equal>(node0, node1);
				case AST::NodeType::And:         return expression_operator_binary_nofloat<bitwise_and>(node0, node1);
				case AST::NodeType::Xor:         return expression_operator_binary_nofloat<bitwise_xor>(node0, node1);
				case AST::NodeType::Xnor:        return expression_operator_binary_nofloat<bitwise_xnor>(node0, node1);
				case AST::NodeType::Or:          return expression_operator_binary_nofloat<bitwise_or>(node0, node1);
				case AST::NodeType::Land:        return expression_operator_binary<logical_and>(node0, node1);
				case AST::NodeType::Lor:         return expression_operator_binary<logical_or>(node0, node1);
				default:
					return nullptr;
				}
			}

			AST::Node::Ptr ExpressionEvaluation::evaluate_constant_node(const AST::NodeType operation,
			                                                            const AST::Node::Ptr node0,
			                                                            const AST::Node::Ptr node1,
			                                                            const AST::Node::Ptr node2) {
				switch(operation) {
				case AST::NodeType::Cond:  return expression_operator_ternary<ternary>(node0, node1, node2);
				case AST::NodeType::PartselectMinusIndexed:
					{
						partselect_minus<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(node0),
						          AST::cast_to<AST::IntConstN>(node1),
						          AST::cast_to<AST::IntConstN>(node2));
					}

				case AST::NodeType::PartselectPlusIndexed:
					{
						partselect_plus<AST::IntConstN::Ptr> op;
						return op(AST::cast_to<AST::IntConstN>(node0),
						          AST::cast_to<AST::IntConstN>(node1),
						          AST::cast_to<AST::IntConstN>(node2));
					}
				default:
					return nullptr;
				}
			}

		}
	}
}
