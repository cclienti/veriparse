#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/instance.hpp>
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
	if (!node) {
		return 0;
	}

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

	return 0;
}


int ModuleInstanceNormalizer::split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (!node) {
		return 0;
	}

	switch (node->get_node_type()) {
	case AST::NodeType::Instancelist:
		{
			const auto &instancelist = AST::cast_to<AST::Instancelist>(node);
			const auto &instances = instancelist->get_instances();

			if (!instances) {
				LOG_ERROR_N(node) << "instance list is null";
				return 1;
			}

			if (instances->size() != 1) {
				LOG_ERROR_N(node) << "more than one element in the instance list, something failed during split";
				return 1;
			}

			const auto &instance = instances->front();
			const auto &array = instance->get_array();

			if (!array) {
				// Nothing to split
				return 0;
			}

			Analysis::Dimensions::DimInfo dim_inst;
			if (!Analysis::Dimensions::extract_array(array, Analysis::Dimensions::Packing::unpacked, dim_inst)) {
				LOG_WARNING_N(node) << "could not split instance array, dimensions cannot be resolved";
				return 1;
			}

			const auto &portlist = instance->get_portlist();

			if (!portlist) {
				// No ports declared, we just have to split the
				// instance.

				auto instlistlist = std::make_shared<AST::Node::List>();
				for (std::size_t i=0; i<dim_inst.width; i++) {
					const auto &new_name = instance->get_name() + std::to_string(dim_inst.msb-i);
					const auto &new_instlist = AST::cast_to<AST::Instancelist>(node->clone());
					const auto &new_inst = new_instlist->get_instances()->front();
					new_inst->set_array(nullptr);
					new_inst->set_name(new_name);
					instlistlist->emplace_back(new_instlist);
				}

				parent->replace(node, instlistlist);
				return 0;
			}

			// Check portarg declaration in instance
			std::size_t count_portarg = 0;
			for (const auto &value: *portlist) {
				if (!value->get_name().empty()) {
					count_portarg++;
				}
			}

			if (count_portarg != 0 && portlist->size() != count_portarg) {
				LOG_ERROR_N(instance) << "could not mix non-named and named ports in instance";
				return 1;
			}

			// Vector of pair<value node, parent for the value>.
			// Usually the value is an identifier or an aggregation
			// operator {} and the parent is the corresponding Portarg
			// or the instance itself if no Portargs are employed.
			// std::vector< std::pair<AST::Node::Ptr, AST::Node::Ptr> > vec_value_parent;

			auto instlistlist = std::make_shared<AST::Node::List>();

			for (std::size_t i=0; i<dim_inst.width; i++) {
				const auto &new_name = instance->get_name() + std::to_string(dim_inst.msb-i);
				const auto &new_instlist = AST::cast_to<AST::Instancelist>(node->clone());
				const auto &new_inst = new_instlist->get_instances()->front();
				new_inst->set_array(nullptr);
				new_inst->set_name(new_name);
				instlistlist->emplace_back(new_instlist);

				// We must check each port if there is and index "[]"
				// to add.
				for (auto &port: *new_inst->get_portlist()) {
					const auto &arg = port->get_value();

					// The argument is an identifier, we can go
					// further. Concat are excluded.
					if (arg->is_node_type(AST::NodeType::Identifier)) {
						const auto &id_name = AST::cast_to<AST::Identifier>(arg)->get_name();

						// We search for the identifier name in the map.
						auto itsearch = m_dim_map.find(id_name);
						if (itsearch != m_dim_map.end()) {
							const auto &id_dim_info = itsearch->second.list.back();

							if (!id_dim_info.is_packed && id_dim_info.width != dim_inst.width) {
								LOG_ERROR_N(port) << "Bad outer dimension of " << id_name << " (unpacked), "
								                  << "expecting " << dim_inst.width << ", received " << id_dim_info.width;
								return 1;
							}

							std::size_t width_mod = id_dim_info.width % dim_inst.width;
							if (width_mod != 0) {
								LOG_ERROR_N(port) << "Bad outer dimension of " << id_name;
								return 1;
							}

							AST::Node::Ptr pointer_node;

							std::size_t width_div = id_dim_info.width / dim_inst.width;
							if (width_div == 1) {
								auto index_node = std::make_shared<AST::IntConstN>(10, -1, true, id_dim_info.msb-i);
								pointer_node = std::make_shared<AST::Pointer>(index_node, arg);
							}
							else {
								std::size_t msb_index = id_dim_info.msb-(i*width_div);
								std::size_t lsb_index = msb_index-width_div+1;
								auto msb_index_node = std::make_shared<AST::IntConstN>(10, -1, true, msb_index);
								auto lsb_index_node = std::make_shared<AST::IntConstN>(10, -1, true, lsb_index);
								pointer_node = std::make_shared<AST::Partselect>(msb_index_node, lsb_index_node, arg);
							}

							port->set_value(pointer_node);
						}
						else {
							LOG_INFO_N(port) << "map lookup failed";
						}
					}
					else {
						LOG_INFO_N(port) << "other than identifier";
					}
				}
			}

			parent->replace(node, instlistlist);


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

	return 0;
}


}
}
}
