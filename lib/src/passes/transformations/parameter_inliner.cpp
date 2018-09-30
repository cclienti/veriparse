#include <veriparse/passes/transformations/parameter_inliner.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			ParameterInliner::ParameterInliner():
				m_paramlist (nullptr),
				m_paramlist_inst (nullptr)
			{
			}

			ParameterInliner::ParameterInliner(AST::ParamArg::ListPtr paramlist_inst):
				m_paramlist (nullptr),
				m_paramlist_inst (paramlist_inst)
			{
			}

			int ParameterInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent) {
				m_paramlist = Analysis::Module::get_parameter_nodes(node);
				if (m_paramlist) {
					int ret;
					if((ret=resolve_paramlist(node))) return ret;
				}

				m_paramlist = Analysis::Module::get_parameter_nodes(node);
				if (m_paramlist) {
					ASTReplace::ReplaceMap rmap;
					for (AST::Parameter::Ptr p: *m_paramlist) {
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
						remove_parameter(node, elt.first, parent);
					}
				}

				return 0;
			}

			int ParameterInliner::resolve_paramlist(AST::Node::Ptr node) {
				// if m_paramlist_inst is not null, we replace
				// corresponding values in paramlist by those given in
				// m_paramlist_inst
				if (m_paramlist_inst) {
					for (AST::ParamArg::Ptr pinst: *m_paramlist_inst) {
						auto result =
							std::find_if(std::begin(*m_paramlist), std::end(*m_paramlist),
							             [&] (AST::Parameter::Ptr const& p)
							             {return p->get_name() == pinst->get_name();});

						if (result != std::end(*m_paramlist)) {
							AST::Parameter::Ptr p = *result;
							p->set_value(pinst->get_value());
						}
					}
				}

				// Inline parameters rvalue in all others parameters. The algorithm is
				// in O(n^2). For each parameter, we replace the parameter rvalue
				// in all other parameters.
				for (AST::Parameter::Ptr pa: *m_paramlist) {
					for (AST::Parameter::Ptr pb: *m_paramlist) {
						if (pa->get_name() != pb->get_name()) {
							auto val = pa->get_value();
							if (val)
							{
								auto var = AST::cast_to<AST::Rvalue>(val)->get_var();
								ASTReplace::replace_identifier(pb, pa->get_name(), var);
							}
							else {
								LOG_WARNING_N(node) << "missing value for parameter " << pa->get_name();
							}
						}
					}
				}

				return 0;
			}


			int ParameterInliner::remove_parameter(AST::Node::Ptr node, std::string name, AST::Node::Ptr parent) {
				if (node) {
					switch(node->get_node_type()) {
					case AST::NodeType::Parameter:
						if((parent) && (AST::cast_to<AST::Parameter>(node)->get_name() == name)) {
							parent->remove(node);
						}
						break;

					case AST::NodeType::Function:  break;
					case AST::NodeType::Task:      break;

					default:
						{
							AST::Node::ListPtr children = node->get_children();
							for (AST::Node::Ptr child: *children) {
								remove_parameter(child, name, node);
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
