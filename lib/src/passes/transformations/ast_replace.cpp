#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			int ASTReplace::replace_identifier(AST::Node::Ptr node, const std::string &id_name,
			                                   AST::Node::Ptr value, AST::Node::Ptr parent) {
				return replace_identifier(node, {{id_name, value}}, parent);
			}

			int ASTReplace::replace_identifier(AST::Node::ListPtr node_list,
			                                   const std::string &id_name, AST::Node::Ptr value) {
				int ret = 0;

				if(node_list) {
					for(AST::Node::Ptr node: *node_list) {
						ret += ASTReplace::replace_identifier(node, id_name, value, nullptr);
					}
				}
				else {
					LOG_ERROR << "Empty node list";
					return 1;
				}

				return ret;
			}

			int ASTReplace::replace_identifier(AST::Node::Ptr node, const ReplaceMap &replace_map,
			                                   AST::Node::Ptr parent) {
				int ret = 0;

				if (replace_map.size() == 0) {
					return 0;
				}

				if (node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Identifier:
						{
							const std::string identifier_name = AST::cast_to<AST::Identifier>(node)->get_name();
							ReplaceMap::const_iterator it = replace_map.find(identifier_name);
							if(it != replace_map.cend()) {
								parent->replace(node, it->second->clone());
							}
						}
						break;

					case AST::NodeType::Function:  break;
					case AST::NodeType::Task:      break;

					default:
						{
							AST::Node::ListPtr children = node->get_children();
							for (AST::Node::Ptr child: *children) {
								ret += replace_identifier(child, replace_map, node);
							}
						}
					}
				}
				else {
					LOG_ERROR << "Empty node, parent is " << parent;
					return 1;
				}

				return ret;
			}

			int ASTReplace::replace_identifier(AST::Node::ListPtr node_list, const ReplaceMap &replace_map) {
				int ret = 0;

				if(node_list) {
					for(AST::Node::Ptr node: *node_list) {
						ret += ASTReplace::replace_identifier(node, replace_map, nullptr);
					}
				}
				else {
					LOG_ERROR << "Empty node list";
					return 1;
				}

				return ret;
			}

		}
	}
}
