#include <iostream>
#include <algorithm>
#include <cctype>

#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_operators.hpp>

#if 0
namespace Veriparse {
	namespace AST {

		namespace {
			std::string delete_surrounding_brackets(const std::string &expression_str) {
				std::string result(expression_str);
				if ((result.front() == '(') && (result.back() == ')')) {
					result = result.substr(1, result.size() - 2);
				}
				return result;
			}

			std::string remove_spaces(const std::string &str) {
				std::string result(str);
				result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
				return result;
			}

			std::string remove_whitespaces(const std::string &str) {
				std::string result(str);
				result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
				return result;
			}

			std::string escape(const std::string &str) {
				std::string result(str);
				if ((result.front() == '\\') && (result.back() != ' ')) {
					result.push_back(' ');
				}
				return result;
			}

			std::string unary_operators_to_string(NodeType node_type, const Node::Ptr &right) {
				std::string result;
				result = "(" + std::string(NodeOperators::get_char(node_type)) + right->to_string() + ")";
				return result;
			}

			std::string operators_to_string(NodeType node_type, const Node::Ptr &left, const Node::Ptr &right) {
				std::string result;
				int current_priority = NodeOperators::get_priority(node_type);
				int left_priority = NodeOperators::get_priority(left->get_node_type());
				int right_priority = NodeOperators::get_priority(right->get_node_type());
				std::string left_str = left->to_string();
				std::string right_str = right->to_string();

				if (current_priority > left_priority) {
					left_str = delete_surrounding_brackets(left_str);
				}

				if (current_priority > right_priority) {
					right_str = delete_surrounding_brackets(right_str);
				}

				result = "(" + left_str + " " + std::string(NodeOperators::get_char(node_type)) + " " + right_str + ")";

				return result;
			}

		}



		std::string Source::to_string(void) const {
			return get_description()->to_string();
		}

		std::string Description::to_string(void) const {
			std::string result;
			for(const Node::Ptr &node : *get_definitions()) {
				result.append(node->to_string());
			}
			return result;
		}

		std::string Pragma::to_string(void) const {
			std::string result;
			const Node::Ptr &expression = get_expression();
			if (expression) {
				result = "(* " + get_name() + " = " + expression->to_string() + " *)";
			}
			else {
				result = "(* " + get_name() + " *)";
			}
			return result;
		}

		std::string Module::to_string(void) const {
			return std::string();
		}

		std::string ParamList::to_string(void) const {
			return std::string();
		}

		std::string PortList::to_string(void) const {
			return std::string();
		}

		std::string Port::to_string(void) const {
			return std::string();
		}

		std::string Width::to_string(void) const {
			return std::string();
		}

		std::string Length::to_string(void) const {
			return std::string();
		}

		std::string Identifier::to_string(void) const {
			return std::string();
		}

		std::string IntConst::to_string(void) const {
			return std::string();
		}

		std::string FloatConst::to_string(void) const {
			return std::string();
		}

		std::string StringConst::to_string(void) const {
			return std::string();
		}

		std::string Input::to_string(void) const {
			return std::string();
		}

		std::string Output::to_string(void) const {
			return std::string();
		}

		std::string Inout::to_string(void) const {
			return std::string();
		}

		std::string Tri::to_string(void) const {
			return std::string();
		}

		std::string Wire::to_string(void) const {
			return std::string();
		}

		std::string Reg::to_string(void) const {
			return std::string();
		}

		std::string Integer::to_string(void) const {
			return std::string();
		}

		std::string Real::to_string(void) const {
			return std::string();
		}

		std::string Genvar::to_string(void) const {
			return std::string();
		}

		std::string WireArray::to_string(void) const {
			return std::string();
		}

		std::string RegArray::to_string(void) const {
			return std::string();
		}

		std::string Ioport::to_string(void) const {
			return std::string();
		}

		std::string Parameter::to_string(void) const {
			return std::string();
		}

		std::string Localparam::to_string(void) const {
			return std::string();
		}

		std::string Supply::to_string(void) const {
			return std::string();
		}

		std::string Decl::to_string(void) const {
			return std::string();
		}

		std::string Concat::to_string(void) const {
			return std::string();
		}

		std::string LConcat::to_string(void) const {
			return std::string();
		}

		std::string Repeat::to_string(void) const {
			return std::string();
		}

		std::string Partselect::to_string(void) const {
			return std::string();
		}

		std::string PartselectPlusIndexed::to_string(void) const {
			return std::string();
		}

		std::string PartselectMinusIndexed::to_string(void) const {
			return std::string();
		}

		std::string Pointer::to_string(void) const {
			return std::string();
		}

		std::string Lvalue::to_string(void) const {
			return std::string();
		}

		std::string Rvalue::to_string(void) const {
			return std::string();
		}

		std::string Uplus::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Uminus::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Ulnot::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Unot::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Uand::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Unand::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Uor::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Unor::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Uxor::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Uxnor::to_string(void) const {
			return unary_operators_to_string(get_node_type(), get_right());
		}

		std::string Power::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Times::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Divide::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Mod::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Plus::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Minus::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Sll::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Srl::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Sra::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string LessThan::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string GreaterThan::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string LessEq::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string GreaterEq::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Eq::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string NotEq::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Eql::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string NotEql::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string And::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Xor::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Xnor::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Or::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Land::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Lor::to_string(void) const {
			return operators_to_string(get_node_type(), get_left(), get_right());
		}

		std::string Cond::to_string(void) const {
			return std::string();
		}

		std::string Always::to_string(void) const {
			return std::string();
		}

		std::string SensList::to_string(void) const {
			return std::string();
		}

		std::string Sens::to_string(void) const {
			return std::string();
		}

		std::string Assign::to_string(void) const {
			return std::string();
		}

		std::string Substitution::to_string(void) const {
			return std::string();
		}

		std::string BlockingSubstitution::to_string(void) const {
			return std::string();
		}

		std::string NonblockingSubstitution::to_string(void) const {
			return std::string();
		}

		std::string IfStatement::to_string(void) const {
			return std::string();
		}

		std::string ForStatement::to_string(void) const {
			return std::string();
		}

		std::string WhileStatement::to_string(void) const {
			return std::string();
		}

		std::string CaseStatement::to_string(void) const {
			return std::string();
		}

		std::string CasexStatement::to_string(void) const {
			return std::string();
		}

		std::string Case::to_string(void) const {
			return std::string();
		}

		std::string Block::to_string(void) const {
			return std::string();
		}

		std::string Initial::to_string(void) const {
			return std::string();
		}

		std::string EventStatement::to_string(void) const {
			return std::string();
		}

		std::string WaitStatement::to_string(void) const {
			return std::string();
		}

		std::string ForeverStatement::to_string(void) const {
			return std::string();
		}

		std::string DelayStatement::to_string(void) const {
			return std::string();
		}

		std::string InstanceList::to_string(void) const {
			return std::string();
		}

		std::string Instance::to_string(void) const {
			return std::string();
		}

		std::string ParamArg::to_string(void) const {
			return std::string();
		}

		std::string PortArg::to_string(void) const {
			return std::string();
		}

		std::string Function::to_string(void) const {
			return std::string();
		}

		std::string FunctionCall::to_string(void) const {
			return std::string();
		}

		std::string Task::to_string(void) const {
			return std::string();
		}

		std::string TaskCall::to_string(void) const {
			return std::string();
		}

		std::string GenerateStatement::to_string(void) const {
			return std::string();
		}

		std::string SystemCall::to_string(void) const {
			return std::string();
		}

		std::string IdentifierScopeLabel::to_string(void) const {
			return std::string();
		}

		std::string IdentifierScope::to_string(void) const {
			return std::string();
		}

		std::string Disable::to_string(void) const {
			return std::string();
		}

		std::string ParallelBlock::to_string(void) const {
			return std::string();
		}

		std::string SingleStatement::to_string(void) const {
			std::string result = get_statement()->to_string();
			return result + ";";
		}

	}
}
#endif
