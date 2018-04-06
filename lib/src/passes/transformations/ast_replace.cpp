#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			int ASTReplace::replace_identifier(AST::Node::Ptr node, const std::string &id_name,
			                                   AST::Node::Ptr value, AST::Node::Ptr parent) {
				if (node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Identifier:
						if(parent) {
							if (parent->get_node_type() != AST::NodeType::Lvalue) {
								if (AST::cast_to<AST::Identifier>(node)->get_name() == id_name) {
									parent->replace(node, value->clone());
								}
							}
						}
						break;

					case AST::NodeType::Function:  break;
					case AST::NodeType::Task:      break;

					default:
						{
							AST::Node::ListPtr children = node->get_children();
							for (AST::Node::Ptr child: *children) {
								replace_identifier(child, id_name, value, node);
							}
						}
					}
				}
				else {
					LOG_ERROR << "Empty node, parent is " << parent;
					return 1;
				}

				return 0;
			}

			int ASTReplace::replace_identifier(AST::Node::ListPtr node_list,
			                                   const std::string &id_name, AST::Node::Ptr value) {
				if(node_list) {
					for(AST::Node::Ptr node: *node_list) {
						ASTReplace::replace_identifier(node, id_name, value, nullptr);
					}
				}
				else {
					return 1;
				}

				return 0;
			}

			int ASTReplace::replace_identifier(AST::Node::Ptr node, const ReplaceMap &replace_map,
			                                   AST::Node::Ptr parent) {
				for(auto &elt: replace_map) {
					ASTReplace::replace_identifier(node, elt.first, elt.second, nullptr);
				}

				return 0;
			}

			int ASTReplace::replace_identifier(AST::Node::ListPtr node_list, const ReplaceMap &replace_map) {
				if(node_list) {
					for(AST::Node::Ptr node: *node_list) {
						ASTReplace::replace_identifier(node, replace_map, nullptr);
					}
				}
				else {
					return 1;
				}

				return 0;
			}

		}
	}
}
