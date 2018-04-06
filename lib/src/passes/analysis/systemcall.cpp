#include <veriparse/passes/analysis/systemcall.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr SystemCall::get_argument_identifier_nodes(AST::SystemCall::Ptr systemcall) {
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(systemcall, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> SystemCall::get_argument_identifier_names(AST::SystemCall::Ptr systemcall) {
				AST::Identifier::ListPtr identifiers = get_argument_identifier_nodes(systemcall);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

		}
	}
}
