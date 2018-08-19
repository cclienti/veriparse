#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {

int ModuleInstanceNormalizer::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int ret = 0;

	ret  = split_lists(node, parent);
	ret += analyze_signals(node, parent);
	ret += split_array(node, parent);

	return ret;
}

int ModuleInstanceNormalizer::split_lists(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Instancelist:
			{
				const auto &new_instance_lists = std::make_shared<AST::Node::List>();

				const auto &instance_list = AST::cast_to<AST::Instancelist>(node);
				for (const auto &inst: *instance_list->get_instances()) {
					const auto &new_instance_list = std::make_shared<AST::Instancelist>();
					new_instance_list->set_filename(node->get_filename());
					new_instance_list->set_line(node->get_line());
					new_instance_list->set_module(instance_list->get_module());

					// Clone parameters to populate the new_instance_list
					const auto &new_params = AST::ParamArg::clone_list(inst->get_parameterlist());
					new_instance_list->set_parameterlist(new_params);

					// Clone instance into a new list of instances
					const auto &new_instances = std::make_shared<AST::Instance::List>();
					if (inst) {
						new_instances->push_back(AST::cast_to<AST::Instance>(inst->clone()));
					}

					// Insert the instance list in the node InstanceList
					new_instance_list->set_instances(new_instances);

					// Keep the result
					new_instance_lists->push_back(new_instance_list);
				}
				parent->replace(node, new_instance_lists);
			}
			break;

		default:
			int ret = 0;
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += split_lists(child, node);
			}
			return ret;
		}
	}

	return 0;
}

int ModuleInstanceNormalizer::analyze_signals(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Module:
			{
				const auto &var_nodes = Analysis::Module::get_variable_nodes(node);
				const auto &io_nodes = Analysis::Module::get_iodir_nodes(node);

				for (const auto &io: *io_nodes) {
					LOG_INFO << io;
					const auto &widths = io->get_widths();
					if (widths) {
						DimensionList dim_list;
						mpz_class msb, lsb;
						for (const auto &width: *widths) {
							const bool msb_valid = ExpressionEvaluation().evaluate_node(width->get_msb(), msb);
							const bool lsb_valid = ExpressionEvaluation().evaluate_node(width->get_lsb(), lsb);
							if (msb_valid && lsb_valid) {
								Dimension dim;
								dim.msb = msb.get_ui();
								dim.lsb = msb.get_ui();
								dim.width = std::max(dim.msb, dim.lsb) - std::min(dim.msb, dim.lsb) + 1;
								dim.is_big = dim.msb > dim.lsb;
								dim.is_packed = true;
								dim_list.emplace_back(dim);
							}
						}
						if (!dim_list.empty()) {
							auto ret = m_signals.emplace(std::make_pair(io->get_name(), dim_list));
							auto key_val_it = ret.first;
							bool is_inserted = ret.second;
							// if (!is_inserted && key_val_it->second != dim_list) {
							// 	LOG_ERROR << "";
							// 	return 1;
							// }
						}
					}
				}

				for (const auto &var: *var_nodes) {
					LOG_INFO << var;
				}

			}
			break;

		default:
			int ret = 0;
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += analyze_signals(child, node);
			}
			return ret;
		}
	}

	return 0;
}

int ModuleInstanceNormalizer::split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Instance:
			{
			}
			break;

		default:
			int ret = 0;
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += split_array(child, node);
			}
			return ret;
		}
	}

	return 0;
}


}
}
}
