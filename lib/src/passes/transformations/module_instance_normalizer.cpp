#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ModuleInstanceNormalizer::ModuleInstanceNormalizer(const Analysis::Module::ModulesMap &modules_map) :
	m_modules_map (modules_map)
{
}

ModuleInstanceNormalizer::~ModuleInstanceNormalizer()
{
}

int ModuleInstanceNormalizer::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	int ret = Analysis::Dimensions::analyze_decls(node, m_dim_map);
	if (ret) {
		LOG_ERROR_N(node) << "error during signal dimensions analysis";
		return ret;
	}

	ret  = split_lists(node, parent);
	ret += set_portarg_names(node, parent);
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

	//-----------------------------------------------
	// Recurse in children
	//-----------------------------------------------

	if (!node->is_node_type(AST::NodeType::Instancelist)) {
		int ret = 0;
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			ret += set_portarg_names(child, node);
		}
		return ret;
	}

	//-----------------------------------------------
	// Get the instance
	//-----------------------------------------------

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

	//-----------------------------------------------
	// Analyze the instance array if it exists
	//-----------------------------------------------

	const auto &array = instance->get_array();

	if (!array) {
		// Nothing to split
		return 0;
	}

	// Extract the instance array dimensions
	Analysis::Dimensions::DimInfo array_dims;
	if (!Analysis::Dimensions::extract_array(array, Analysis::Dimensions::Packing::unpacked, array_dims)) {
		LOG_WARNING_N(node) << "could not split instance array, dimensions cannot be resolved";
		return 1;
	}

	//-----------------------------------------------
	// Lookup for the instantiated module definition
	//-----------------------------------------------

	const auto &module_name = instance->get_module();
	auto itmod = m_modules_map.find(module_name);
	if (itmod == m_modules_map.end()) {
		LOG_ERROR_N(instance) << "Instantiated module """ << module_name << """ not found";
		return 1;
	}

	const auto &module_decl = itmod->second;

	Analysis::Dimensions::DimMap module_dim_map;
	int ret = Analysis::Dimensions::analyze_decls(module_decl, module_dim_map);
	if (ret) {
		LOG_ERROR_N(module_decl) << "error during signal dimensions analysis";
		return ret;
	}

	//-----------------------------------------------
	// Split the instance array but the portlist
	// is empty
	//-----------------------------------------------

	const auto &portlist = instance->get_portlist();

	if (!portlist) {
		// No ports declared, we just have to split the
		// instance.
		auto instlistlist = std::make_shared<AST::Node::List>();

		for (std::size_t i=0; i<array_dims.width; i++) {
			const auto &new_name = instance->get_name() + std::to_string(array_dims.msb-i);
			const auto &new_instlist = AST::cast_to<AST::Instancelist>(node->clone());
			const auto &new_inst = new_instlist->get_instances()->front();
			new_inst->set_array(nullptr);
			new_inst->set_name(new_name);
			instlistlist->emplace_back(new_instlist);
		}

		parent->replace(node, instlistlist);
		return 0;
	}

	//-----------------------------------------------
	// Split the instance array
	//-----------------------------------------------

	auto instlistlist = std::make_shared<AST::Node::List>();

	for (std::size_t i=0; i<array_dims.width; i++) {
		const auto &new_name = instance->get_name() + std::to_string(array_dims.msb-i);
		const auto &new_instlist = AST::cast_to<AST::Instancelist>(node->clone());
		const auto &new_inst = new_instlist->get_instances()->front();
		new_inst->set_array(nullptr);
		new_inst->set_name(new_name);
		instlistlist->emplace_back(new_instlist);

		for (auto &port: *new_inst->get_portlist()) {
			// Check argument value dimensions
			const auto &value = port->get_value();
			Analysis::Dimensions::DimList value_dims;
			Analysis::Dimensions::analyze_expr(value, m_dim_map, value_dims);

			// Check in the module declaration for the corresponding port dimension
			const auto &arg = port->get_name();
			auto itarg = module_dim_map.find(arg);
			if (itarg == module_dim_map.end()) {
				LOG_ERROR_N(port) << "cannot find port '" << arg << "' in module '" << module_name << "'";
				return 1;
			}
			const auto &arg_dims = itarg->second;

			// At this point, we have the array_dims, the value_dims and
			// the arg_dims. We can check if we must index the port value
			// or not. If yes we will add a pointer or partselect to the
			// value even if it is not syntactically correct. This will
			// be corrected in the final step.

#if 0
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
						long msb_index = id_dim_info.msb-(i*width_div);
						long lsb_index = msb_index-width_div+1;

						if (msb_index < 0 || lsb_index < 0) {
							LOG_ERROR_N(port) << "bad instance index, negative range in port argument";
						}

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
#endif

		}
	}

	parent->replace(node, instlistlist);

	return 0;
}


int ModuleInstanceNormalizer::set_portarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (!node) {
		return 0;
	}

	//-----------------------------------------------
	// Recurse in children
	//-----------------------------------------------

	if (!node->is_node_type(AST::NodeType::Instancelist)) {
		int ret = 0;
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			ret += set_portarg_names(child, node);
		}
		return ret;
	}

	//-----------------------------------------------
	// Extract instance information
	//-----------------------------------------------

	const auto &instancelist = AST::cast_to<AST::Instancelist>(node);
	const auto &instances = instancelist->get_instances();

	if (!instances) {
		LOG_ERROR_N(node) << "instance list is null";
		return 1;
	}

	if (instances->size() != 1) {
		LOG_ERROR_N(node) << "instances not correctly splitted";
		return 1;
	}

	const auto &instance = instances->front();
	const auto &portlist = instance->get_portlist();

	//-----------------------------------------------
	// Extract instance module information
	//-----------------------------------------------

	const auto &inst_module_str = instance->get_module();

	auto itm = m_modules_map.find(inst_module_str);
	if (itm == m_modules_map.end()) {
		LOG_ERROR_N(instance) << "Instantiated module """ << inst_module_str << """ not found";
		return 1;
	}

	const auto &module_decl = itm->second;
	auto decl_portnames = Analysis::Module::get_port_names(module_decl);

	//-----------------------------------------------
	// Normalize port arguments
	//-----------------------------------------------

	// If no instance arguments are declared, look in the module
	// declaration to specify names.
	if (!portlist) {
		const auto new_portlist = std::make_shared<AST::PortArg::List>();
		for (const auto &argname: decl_portnames) {
			const auto &missing = std::make_shared<AST::PortArg>(nullptr, argname,
			                                                     instance->get_filename(), instance->get_line());
			new_portlist->push_back(missing);
		}
		if (new_portlist->size() != 0) {
			instance->set_portlist(new_portlist);
		}
		return 0;
	}

	// Check if there is a mix between unnamed and named arguments.
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

	if (count_portarg == 0) {
		// Add all port argument names in the order of appearance.

		// Check that all arguments are specified.
		if (decl_portnames.size() != portlist->size()) {
			LOG_ERROR_N(module_decl) << "missing instance some arguments";
			return 1;
		}
		auto it = decl_portnames.cbegin();
		for (const auto &portarg: *portlist) {
			portarg->set_name(*it++);
		}
	}
	else {
		// Add missing port arg names.

		std::vector<std::string> args_to_add;
		// Search of the matching instanciated portarg name and the
		// declared portarg name (in the module declaration).
		for (const auto &portarg : *portlist) {
			const auto &argname = portarg->get_name();
			auto it = decl_portnames.cbegin();
			for (; it != decl_portnames.cend(); ++it) {
				if (argname == *it) {
					break;
				}
			}
			if (it != decl_portnames.cend()) {
				// Name found, we can remove the name to avoid searching
				// for it later.
				decl_portnames.erase(it);
			}
		}

		// add all name not found in the portlist
		for (const auto &argname: decl_portnames)
		{
			const auto &missing = std::make_shared<AST::PortArg>(nullptr, argname,
			                                                     instance->get_filename(), instance->get_line());
			portlist->push_back(missing);
		}
	}

	// //-----------------------------------------------
	// // Replace port arguments by identifier only
	// //-----------------------------------------------

	// const auto &stmts = std::make_shared<AST::Node::List>();

	// std::size_t portindex = 0;
	// for (const auto &portarg: *portlist) {
	// 	// Construct a name for the new signal. TODO: check if a signal with this name already exists.
	// 	const std::string portname = instance->get_name() + "_" + portarg->get_name() + std::to_string(portindex++);

	// 	// If the port is empty continue
	// 	const auto &portvalue = portarg->get_value();
	// 	if (!portvalue) {
	// 		continue;
	// 	}

	// 	// If the portvalue is an identifier we have nothing to do.
	// 	if (portvalue->is_node_type(AST::NodeType::Identifier)) {
	// 		continue;
	// 	}

	// 	// Analyse portvalue dimensions
	// 	Analysis::Dimensions::DimList value_dims;
	// 	if (Analysis::Dimensions::analyze_expr(portvalue, m_dim_map, value_dims)) {
	// 		return 1;
	// 	}

	// 	if (!value_dims.is_fully_packed()) {
	// 		LOG_ERROR_N(portvalue) << "port value is not packed";
	// 		return 1;
	// 	}

	// 	// Get portarg dimensions from instantiated module declaration
	// 	const auto &arg_dims = inst_module_dim_map[portarg->get_name()];
	// 	if (!arg_dims.is_fully_packed()) {
	// 		LOG_ERROR_N(portarg) << "port is not packed";
	// 		return 1;
	// 	}

	// 	if (arg_dims.packed_width() != value_dims.packed_width()) {
	// 		LOG_WARNING_N(portvalue) << "port value width (" << value_dims.packed_width()
	// 		                         << ") is not consistant with "
	// 		                         << "port '" << portarg->get_name() << "' of "
	// 		                         << "module '" << inst_module->get_name() << "' "
	// 		                         << "(" << arg_dims.packed_width() << ")";
	// 	}

	// 	// Generate the new identifier declaration
	// 	const auto &decl = Analysis::Dimensions::generate_decl(portname, AST::NodeType::Wire, arg_dims);
	// 	if (!decl) {
	// 		return 1;
	// 	}

	// 	const std::string &filename = portarg->get_filename();
	// 	const std::uint32_t line = portarg->get_line();
	// 	const auto &identifier = std::make_shared<AST::Identifier>(nullptr, portname, filename, line);
	// 	const auto &lvalue = std::make_shared<AST::Lvalue>(identifier, filename, line);
	// 	const auto &rvalue = std::make_shared<AST::Rvalue>(portvalue, filename, line);
	// 	const auto &assign = std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr, filename, line);

	// 	m_dim_map.emplace(portname, arg_dims);
	// 	portarg->set_value(std::make_shared<AST::Identifier>(nullptr, portname, filename, line));
	// 	stmts->push_back(decl);
	// 	stmts->push_back(assign);
	// }

	// if (stmts->empty()) {
	// 	return 0;
	// }

	// stmts->push_back(node);
	// parent->replace(node, stmts);

	return 0;
}


}
}
}
