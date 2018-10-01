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
				if (!m_localparamlist) {
					return 0;
				}

				int ret = resolve_localparamlist();
				if (ret != 0) {
					return ret;
				}

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

				// We does not know what localparam match in the
				// replace_identifier. This is not completely safe to
				// remove localparams. But if it fails, the code is not
				// syntactically correct.
				for (const auto &elt: rmap) {
					remove_localparam(node, elt.first, parent);
				}

				return 0;
			}

			int LocalparamInliner::resolve_localparamlist() {
				// Inline parameters rvalue in all others localparams. The
				// algorithm is in O(n^2). For each localparam, we replace
				// the localparam rvalue in all other localparams.
				for (const auto &pa: *m_localparamlist) {
					for (const auto &pb: *m_localparamlist) {
						if (pa->get_name() != pb->get_name()) {
							auto val = pa->get_value();
							if (val)	{
								auto var = AST::cast_to<AST::Rvalue>(val)->get_var();
								ASTReplace::replace_identifier(pb, pa->get_name(), var);
							}
							else {
								LOG_WARNING_N(pa) << "missing value for localparam " << pa->get_name();
							}
						}
					}
				}

				return 0;
			}

			int LocalparamInliner::remove_localparam(const AST::Node::Ptr &node, const std::string &name,
			                                         const AST::Node::Ptr &parent) {
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
