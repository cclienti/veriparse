#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr Instance::get_argument_identifier_nodes(AST::Instance::Ptr instance) {
				AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(instance, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> Instance::get_argument_identifier_names(AST::Instance::Ptr instance) {
				AST::Identifier::ListPtr identifiers = get_argument_identifier_nodes(instance);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

		}
	}
}
