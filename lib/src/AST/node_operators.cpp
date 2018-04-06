#include <veriparse/AST/node_operators.hpp>

namespace Veriparse {
	namespace AST {

		const char *NodeOperators::get_char(NodeType node_type) {
			switch(node_type) {
			case NodeType::Uplus:  return "+";
			case NodeType::Uminus: return "-";
			case NodeType::Ulnot:  return "!";
			case NodeType::Unot:   return "~";
			case NodeType::Uand:   return "&";
			case NodeType::Unand:  return "~&";
			case NodeType::Uor:    return "|";
			case NodeType::Unor:   return "~|";
			case NodeType::Uxor:   return "^";
			case NodeType::Uxnor:  return "~^";

			case NodeType::Power:       return "**";
			case NodeType::Times:       return "*";
			case NodeType::Divide:      return "/";
			case NodeType::Mod:         return "%";
			case NodeType::Plus:        return "+";
			case NodeType::Minus:       return "-";
			case NodeType::Sll:         return "<<";
			case NodeType::Srl:         return ">>";
			case NodeType::Sra:         return ">>>";
			case NodeType::LessThan:    return "<";
			case NodeType::GreaterThan: return ">";
			case NodeType::LessEq:      return "<=";
			case NodeType::GreaterEq:   return ">=";
			case NodeType::Eq:          return "==";
			case NodeType::NotEq:       return "!=";
			case NodeType::Eql:         return "===";
			case NodeType::NotEql:      return "!==";
			case NodeType::And:         return "&";
			case NodeType::Xor:         return "^";
			case NodeType::Xnor:        return "~^";
			case NodeType::Or:          return "|";
			case NodeType::Land:        return "&&";
			case NodeType::Lor:         return "||";
			default: return "";
			}
		}

		int NodeOperators::get_priority(NodeType node_type) {
			switch(node_type) {
			case NodeType::Uplus:  return 0;
			case NodeType::Uminus: return 0;
			case NodeType::Ulnot:  return 0;
			case NodeType::Unot:   return 0;
			case NodeType::Uand:   return 0;
			case NodeType::Unand:  return 0;
			case NodeType::Uor:    return 0;
			case NodeType::Unor:   return 0;
			case NodeType::Uxor:   return 0;
			case NodeType::Uxnor:  return 0;

			case NodeType::Power:       return 1;

			case NodeType::Times:       return 2;
			case NodeType::Divide:      return 2;
			case NodeType::Mod:         return 2;

			case NodeType::Plus:        return 3;
			case NodeType::Minus:       return 3;

			case NodeType::Sll:         return 4;
			case NodeType::Srl:         return 4;
			case NodeType::Sra:         return 4;

			case NodeType::LessThan:    return 5;
			case NodeType::GreaterThan: return 5;
			case NodeType::LessEq:      return 5;
			case NodeType::GreaterEq:   return 5;

			case NodeType::Eq:          return 6;
			case NodeType::NotEq:       return 6;
			case NodeType::Eql:         return 6;
			case NodeType::NotEql:      return 6;

			case NodeType::And:         return 7;
			case NodeType::Xor:         return 8;
			case NodeType::Xnor:        return 8;

			case NodeType::Or:          return 9;

			case NodeType::Land:        return 10;

			case NodeType::Lor:         return 11;

			default: return -1;
			}
		}

	}
}
