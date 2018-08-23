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
	int ret = Analysis::Dimensions::analyze(node, m_dim_map);
	if (ret) {
		LOG_ERROR_N(node) << "error during signal dimensions analysis";
		return ret;
	}

	ret  = split_lists(node, parent);
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


int ModuleInstanceNormalizer::split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (node) {
		switch (node->get_node_type()) {
		case AST::NodeType::Instance:
			{
				const auto &instance = AST::cast_to<AST::Instance>(node);
				const auto &array = instance->get_array();

				if (array) {
					Analysis::Dimensions::DimInfo dim;
					if (Analysis::Dimensions::extract_array(array, Analysis::Dimensions::Packing::unpacked, dim)) {
						LOG_INFO_N(node) << "instance array: " << dim;
					}
					else {
						LOG_WARNING_N(node) << "could not split instance array, dimensions cannot be resolved";
						return 1;
					}
				}
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
