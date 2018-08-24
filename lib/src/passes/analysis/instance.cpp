#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			AST::Identifier::ListPtr Instance::get_argument_identifier_nodes(AST::Instance::Ptr instance) {
				auto list = std::make_shared<AST::Identifier::List>();
				get_node_list<AST::Identifier>(instance, AST::NodeType::Identifier, list);
				return list;
			}

			std::vector<std::string> Instance::get_argument_identifier_names(AST::Instance::Ptr instance) {
				auto identifiers = get_argument_identifier_nodes(instance);
				return get_property_in_list<std::string, AST::Identifier>
					(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
			}

			AST::PortArg::ListPtr Instance::get_port_argument_nodes(AST::Instance::Ptr instance) {
				auto list = std::make_shared<AST::PortArg::List>();
				get_node_list<AST::PortArg>(instance, AST::NodeType::PortArg, list);
				return list;
			}

			std::vector<std::string> Instance::get_port_argument_names(AST::Instance::Ptr instance) {
				auto identifiers = get_port_argument_nodes(instance);
				return get_property_in_list<std::string, AST::PortArg>
					(identifiers, [](AST::PortArg::Ptr n){return n->get_name();});
			}

		}
	}
}
