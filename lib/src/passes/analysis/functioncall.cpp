#include <veriparse/passes/analysis/functioncall.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr FunctionCall::get_argument_identifier_nodes(AST::FunctionCall::Ptr functioncall) {
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(functioncall, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> FunctionCall::get_argument_identifier_names(AST::FunctionCall::Ptr functioncall) {
				AST::Identifier::ListPtr identifiers = get_argument_identifier_nodes(functioncall);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

		}
	}
}
