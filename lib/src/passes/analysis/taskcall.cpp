#include <veriparse/passes/analysis/taskcall.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr TaskCall::get_argument_identifier_nodes(AST::TaskCall::Ptr taskcall) {
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(taskcall, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> TaskCall::get_argument_identifier_names(AST::TaskCall::Ptr taskcall) {
				AST::Identifier::ListPtr identifiers = get_argument_identifier_nodes(taskcall);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

		}
	}
}
