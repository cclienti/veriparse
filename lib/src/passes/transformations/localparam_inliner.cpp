#include <veriparse/passes/transformations/localparam_inliner.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			LocalparamInliner::LocalparamInliner(): m_localparamlist (nullptr) {}

			int LocalparamInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent) {
				m_localparamlist = Analysis::Module::get_localparam_nodes(node);
				if (m_localparamlist) {
					ASTReplace::ReplaceMap rmap;
					for (AST::Localparam::Ptr p: *m_localparamlist) {
						AST::Node::Ptr val = p->get_value();
						if (val) {
							AST::Node::Ptr var = AST::cast_to<AST::Rvalue>(val)->get_var();
							if (var) {
								rmap[p->get_name()] = var;
							}
						}
					}

					ASTReplace::replace_identifier(node, rmap, parent);

					for(const auto &elt: rmap) {
						remove_localparam(node, elt.first, parent);
					}

				}
				return 0;
			}

			int LocalparamInliner::remove_localparam(AST::Node::Ptr node, std::string name, AST::Node::Ptr parent) {
				if (node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Localparam:
						if((parent) && (AST::cast_to<AST::Localparam>(node)->get_name() == name)) {
							parent->remove(node);
						}
						break;

					case AST::NodeType::Function:  break;
					case AST::NodeType::Task:      break;

					default:
						{
							AST::Node::ListPtr children = node->get_children();
							for (AST::Node::Ptr child: *children) {
								remove_localparam(child, name, node);
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

		}
	}
}
