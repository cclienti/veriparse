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
	if (!m_paramlist) {
		return 0;
	}

	int ret = resolve_paramlist();
	if (ret != 0) {
		return ret;
	}

	ASTReplace::ReplaceMap rmap;
	for (const auto &p: *m_paramlist) {
		AST::Node::Ptr val = p->get_value();
		if (val) {
			const auto &var = AST::cast_to<AST::Rvalue>(val)->get_var();
			bool keep_param = false;

			if (m_paramlist_inst) {
				auto search = std::find_if(std::begin(*m_paramlist_inst), std::end(*m_paramlist_inst),
				                           [&] (const AST::ParamArg::Ptr &pinst)
				                           {return p->get_name() == pinst->get_name() && !pinst->get_value();});
				keep_param = (search != std::end(*m_paramlist_inst));
			}

			if (var && !keep_param) {
				rmap[p->get_name()] = var;
			}
		}
	}

	ASTReplace::replace_identifier(node, rmap, parent);

	// We does not know what parameter match in the
	// replace_identifier. This is not completely safe to
	// remove parameters. But if it fails, the code is not
	// syntactically correct.
	for (const auto &elt: rmap) {
		remove_parameter(node, elt.first, parent);
	}

	return 0;
}

int ParameterInliner::resolve_paramlist() {
	// if m_paramlist_inst is not null, we replace
	// corresponding values in paramlist by those given in
	// m_paramlist_inst
	if (m_paramlist_inst) {
		for (const auto &pinst: *m_paramlist_inst) {
			auto result =
				std::find_if(std::begin(*m_paramlist), std::end(*m_paramlist),
				             [&] (AST::Parameter::Ptr const& p)
				             {return p->get_name() == pinst->get_name();});

			if (result != std::end(*m_paramlist)) {
				const auto &p = *result;
				const auto &value = pinst->get_value();
				if (!value) {
					continue;
				}
				else {
					auto rvalue = value;
					if (!value->is_node_type(AST::NodeType::Rvalue)) {
						rvalue = std::make_shared<AST::Rvalue>(value, value->get_filename(), value->get_line());
					}
					p->set_value(rvalue);
				}
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
				if (val) {
					auto var = AST::cast_to<AST::Rvalue>(val)->get_var();
					ASTReplace::replace_identifier(pb, pa->get_name(), var);
				}
				else {
					LOG_WARNING_N(pa) << "missing value for parameter " << pa->get_name();
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
