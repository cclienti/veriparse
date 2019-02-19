#include <veriparse/passes/transformations/module_instance_normalizer.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/misc/math.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>


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
	LOG_INFO_N(node) << "Normalizing module instances";

	LOG_DEBUG_N(node) << "Analyze declaration dimensions";
	int ret = Analysis::Dimensions::analyze_decls(node, m_dim_map);
	if (ret) {
		LOG_ERROR_N(node) << "error during signal dimensions analysis";
		return ret;
	}

	LOG_DEBUG_N(node) << "Analyze unique declarations";
	ret = Analysis::UniqueDeclaration::analyze(node, m_declared);
	if (ret) {
		LOG_ERROR_N(node) << "error during analysis of declared identifiers";
		return ret;
	}

	LOG_DEBUG_N(node) << "Split instance lists";
	ret = split_lists(node, parent);
	if (ret) {
		LOG_ERROR_N(node) << "error during instance list split";
		return ret;
	}

	LOG_DEBUG_N(node) << "Add module port name in instance arguments";
	ret = set_portarg_names(node, parent);
	if (ret) {
		LOG_ERROR_N(node) << "error during instance port name normalization";
		return ret;
	}

	LOG_DEBUG_N(node) << "Add module param name in instance arguments";
	ret = set_paramarg_names(node, parent);
	if (ret) {
		LOG_ERROR_N(node) << "error during instance parameter name normalization";
		return ret;
	}

	LOG_DEBUG_N(node) << "Replace port arg affectation";
	ret = replace_port_affectation(node, parent);
	if (ret) {
		LOG_ERROR_N(node) << "error during instance port value re-assignation";
		return ret;
	}

	LOG_DEBUG_N(node) << "Split arrays";
	ret = split_array(node, parent);
	if (ret) {
		LOG_ERROR_N(node) << "error during instance array split";
		return ret;
	}

	return 0;
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
		{
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
			ret += split_array(child, node);
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
	// Lookup for the instantiated module definition
	//-----------------------------------------------

	const auto &module_name = instance->get_module();
	auto itmod = m_modules_map.find(module_name);
	if (itmod == m_modules_map.end()) {
		// The module is not found, we will keep the instance and we
		// must go on without raising an error.
		LOG_WARNING_N(instance) << "Instantiated module """ << module_name << """ not found";
		return 0;
	}

	const auto &module_decl = itmod->second;

	Analysis::Dimensions::DimMap module_dim_map;
	int ret = Analysis::Dimensions::analyze_decls(module_decl, module_dim_map);
	if (ret) {
		LOG_ERROR_N(module_decl) << "error during signal dimensions analysis";
		return ret;
	}

	//-----------------------------------------------
	// Analyze the instance array if it exists
	//-----------------------------------------------

	const auto &array = instance->get_array();

	if (!array) {
		// Nothing to split
		return 0;
	}

	// Extract the instance array dimensions
	Analysis::Dimensions::DimInfo array_dim;
	if (!Analysis::Dimensions::extract_array(array, Analysis::Dimensions::Packing::packed, array_dim)) {
		LOG_WARNING_N(node) << "could not split instance array, dimensions cannot be resolved";
		return 1;
	}

	//-----------------------------------------------
	// Split the instance array
	//-----------------------------------------------

	auto instlistlist = std::make_shared<AST::Node::List>();

	for (std::int64_t i=array_dim.width-1; i >= 0; i--) {
		std::size_t new_name_index = array_dim.is_big ? array_dim.msb-i : array_dim.msb+i;
		const std::string &new_name = instance->get_name() + std::to_string(new_name_index);
		const auto &new_instlist = AST::cast_to<AST::Instancelist>(node->clone());
		const auto &new_inst = new_instlist->get_instances()->front();
		new_inst->set_array(nullptr);
		new_inst->set_name(new_name);
		instlistlist->emplace_back(new_instlist);

		const auto &portlist = new_inst->get_portlist();
		if (!portlist) {
			continue;
		}

		for (auto &port: *portlist) {
			// Check argument value dimensions
			const auto &value = port->get_value();

			if (!value) {
				// No port value, we go on with the next port.
				continue;
			}
			Analysis::Dimensions::DimList value_dims;
			Analysis::Dimensions::analyze_expr(value, m_dim_map, value_dims);

			// Check in the dimension in the module declaration for the
			// corresponding port.
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
			// value.

			AST::Node::Ptr pnode;

			// The port value does not need to be indexed, nothing to do.
			if (value_dims.is_fully_packed() && arg_dims.packed_width() == value_dims.packed_width()) {
				continue;
			}

			// The port must be indexed.
			auto value_outer_width = value_dims.outer_width();
			auto value_outer_msb = value_dims.outer_msb();
			auto value_outer_is_big = value_dims.outer_is_big();

			std::size_t width_mod = value_outer_width % array_dim.width;
			if (width_mod != 0 || value_dims.list.size() == 0) {
				LOG_ERROR_N(port) << "Bad outer dimension, cannot split instance array";
				return 1;
			}

			std::size_t width_div = value_outer_width / array_dim.width;
			if (width_div == 1) {
				// We just have to pop a dimension, because we will add a pointer node.
				value_dims.list.pop_front();
				if (!value_dims.is_fully_packed() || value_dims.packed_width() != arg_dims.packed_width()) {
					LOG_ERROR_N(port) << "could not index the port value with the declared instance array";
					return 1;
				}

				std::size_t value_outer_index = value_outer_is_big ? value_outer_msb-i : value_outer_msb+i;
				auto index_node = std::make_shared<AST::IntConstN>(10, -1, true,
				                                                   Misc::Math::u64_to_mpz(value_outer_index));
				pnode = std::make_shared<AST::Pointer>(index_node, port->get_value());
			}
			else {
				// We need to slice the outer dimension because we will add
				// a partselect node.
				std::int64_t slice_msb = value_outer_is_big ? (value_outer_msb - width_div*i) : (value_outer_msb + width_div*i);
				std::int64_t slice_lsb = value_outer_is_big ? (slice_msb - width_div + 1) : (slice_msb + width_div - 1);

				if (slice_msb < 0 || slice_lsb < 0) {
					LOG_ERROR_N(port) << "bad instance index during instance array splitting, negative range in port argument";
				}

				value_dims.list.front().msb = slice_msb;
				value_dims.list.front().lsb = slice_lsb;
				value_dims.list.front().width = width_div;

				if (!value_dims.is_fully_packed() || value_dims.packed_width() != arg_dims.packed_width()) {
					LOG_ERROR_N(port) << "could not index the port value with the declared instance array";
					return 1;
				}

				auto slice_msb_node = std::make_shared<AST::IntConstN>(10, -1, true,
				                                                       Misc::Math::i64_to_mpz(slice_msb));
				auto slice_lsb_node = std::make_shared<AST::IntConstN>(10, -1, true,
				                                                       Misc::Math::i64_to_mpz(slice_lsb));
				pnode = std::make_shared<AST::Partselect>(slice_msb_node, slice_lsb_node, port->get_value());
			}

			port->set_value(pnode);

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
		// The module is not found, we will keep the instance and we
		// must go on without raising an error.
		LOG_WARNING_N(instance) << "Instantiated module """ << inst_module_str << """ not found";
		return 0;
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
		// Add missing port arg names. Search of the matching
		// instanciated portarg name and the declared portarg name (in
		// the module declaration).
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
			else {
				LOG_ERROR_N(portarg) << "cannot match port '" << argname << "' "
				                     << "in the module definition";
				return 1;
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

	return 0;
}

int ModuleInstanceNormalizer::set_paramarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
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
			ret += set_paramarg_names(child, node);
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
	const auto &paramlist = instance->get_parameterlist();

	//-----------------------------------------------
	// Extract instance module information
	//-----------------------------------------------

	const auto &inst_module_str = instance->get_module();

	auto itm = m_modules_map.find(inst_module_str);
	if (itm == m_modules_map.end()) {
		// The module is not found, we will keep the instance and we
		// must go on without raising an error.
		LOG_WARNING_N(instance) << "Instantiated module """ << inst_module_str << """ not found";
		return 0;
	}

	const auto &module_decl = itm->second;
	auto decl_paramlist = Analysis::Module::get_parameter_nodes(module_decl);

	//-----------------------------------------------
	// Normalize parameters
	//-----------------------------------------------

	// We have to take care here, because of default parameter value in
	// the instantiated module.

	// If no parameters arguments are declared, we should have to look
	// in the module declaration to analyze default values. It is
	// possible that some parameter values can be not resolved at this
	// point. This step will be done in the module instance inline
	// pass.
	if (!paramlist) {
		if (decl_paramlist->size() == 0) {
			return 0;
		}

		auto new_paramlist = std::make_shared<AST::ParamArg::List>();
		auto new_paramlist2 = std::make_shared<AST::ParamArg::List>();

		for (const auto &param: *decl_paramlist) {
			if (!param->get_value()) {
				LOG_ERROR_N(param) << "missing default value of parameter '" << param->get_name() << "'";
				return 1;
			}
			auto new_param = std::make_shared<AST::ParamArg>(nullptr, param->get_name(),
			                                                 instance->get_filename(), instance->get_line());
			new_paramlist->push_back(new_param);
			new_paramlist2->push_back(AST::cast_to<AST::ParamArg>(new_param->clone()));
		}

		// Parameters of an instance are declared at two places. This
		// would be nice to change this ...
		instance->set_parameterlist(new_paramlist);
		instancelist->set_parameterlist(new_paramlist2);

		return 0;
	}


	// The instance parameter list is not empty
	if (decl_paramlist->size() < paramlist->size()) {
		LOG_ERROR_N(module_decl) << "bad number of parameters for instance '" << instance->get_name() << "', "
		                         << "(module '" << instance->get_module()
		                         << "')";
		return 1;
	}

	// Add missing param arg names. Search of the matching instanciated
	// paramarg name and the declared paramarg name (in the module
	// declaration).
	for (const auto &param : *paramlist) {
		const auto &name = param->get_name();

		if (name.empty()) {
			if (decl_paramlist->empty()) {
				LOG_ERROR_N(param) << "cannot match the parameter in the module declaration";
				return 1;
			}
			param->set_name(decl_paramlist->front()->get_name());
			decl_paramlist->pop_front();
		}
		else {
			auto it = decl_paramlist->cbegin();
			for (; it != decl_paramlist->cend(); ++it) {
				const auto &decl_node = *it;
				if (name == decl_node->get_name()) {
					break;
				}
			}
			if (it != decl_paramlist->cend()) {
				// Name found, we can remove the name to avoid searching
				// for it later.
				decl_paramlist->erase(it);
			}
			else {
				LOG_ERROR_N(param) << "cannot match the parameter '" << name << "' "
				                   << "in the module declaration";
				return 1;
			}
		}
	}

	// add all name not found in the paramlist
	for (const auto &decl_param: *decl_paramlist)
	{
		const auto &missing = std::make_shared<AST::ParamArg>(nullptr, decl_param->get_name(),
		                                                      instance->get_filename(), instance->get_line());
		paramlist->push_back(missing);
	}


	// Parameters of an instance are declared at two places. This would
	// be nice to change this ... So we clear it and we clone the
	// paramlist into paramlist2.

	const auto &paramlist2 = instancelist->get_parameterlist();
	paramlist2->clear();
	for (const auto &param: *paramlist) {
		instancelist->get_parameterlist()->push_back(AST::cast_to<AST::ParamArg>(param->clone()));
	}

	return 0;
}

int ModuleInstanceNormalizer::convert_to_lconcat(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	if (!node) {
		return 0;
	}

	//-----------------------------------------------
	// Recurse in children
	//-----------------------------------------------

	if (!node->is_node_type(AST::NodeType::Concat)) {
		int ret = 0;
		AST::Node::ListPtr children = node->get_children();
		for (AST::Node::Ptr child: *children) {
			ret += convert_to_lconcat(child, node);
		}
		return ret;
	}

	//-----------------------------------------------
	// Convert concat to lconcat
	//-----------------------------------------------

	if (node->is_node_type(AST::NodeType::Concat)) {
		const auto &concat = AST::cast_to<AST::Concat>(node);
		const auto &lconcat = std::make_shared<AST::Lconcat>(concat->get_list(),
		                                                     concat->get_filename(), concat->get_line());
		parent->replace(concat, lconcat);
	}

	return 0;
}

int ModuleInstanceNormalizer::replace_port_affectation(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
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
			ret += replace_port_affectation(child, node);
		}
		return ret;
	}

	//-----------------------------------------------
	// Extract instance information
	//-----------------------------------------------

	auto instancelist = AST::cast_to<AST::Instancelist>(node);
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

	// If the portlist is empty, nothing to do.
	if (!portlist) {
		return 0;
	}

	//-----------------------------------------------
	// Lookup for the instantiated module definition
	//-----------------------------------------------

	const auto &module_name = instance->get_module();
	auto itmod = m_modules_map.find(module_name);
	if (itmod == m_modules_map.end()) {
		// The module is not found, we will keep the instance and we
		// must go on without raising an error.
		LOG_WARNING_N(instance) << "Instantiated module """ << module_name << """ not found";
		return 0;
	}

	const auto &module_decl = itmod->second;

	Analysis::Dimensions::DimMap module_dim_map;
	int ret = Analysis::Dimensions::analyze_decls(module_decl, module_dim_map);
	if (ret) {
		LOG_ERROR_N(module_decl) << "error during signal dimensions analysis";
		return ret;
	}

	const auto &module_decl_outputs = Analysis::Module::get_output_names(module_decl);

	//-----------------------------------------------
	// Analyze the instance array if it exists
	//-----------------------------------------------

	const auto &array = instance->get_array();

	if (!array) {
		// Nothing to split
		return 0;
	}

	// Extract the instance array dimensions
	Analysis::Dimensions::DimInfo array_dim;
	if (!Analysis::Dimensions::extract_array(array, Analysis::Dimensions::Packing::packed, array_dim)) {
		LOG_WARNING_N(node) << "instance array dimensions cannot be resolved";
		return 0;
	}

	//-----------------------------------------------
	// Replace port affectation
	//-----------------------------------------------

	auto new_stmts = std::make_shared<AST::Node::List>();

	for (const auto &port: *portlist) {
		const auto &port_value = port->get_value();
		if (!port_value) {
			continue;
		}

		// Create the identifier node
		const auto &id_str = Analysis::UniqueDeclaration::get_unique_identifier
			(instance->get_name() + "_" + port->get_name(), m_declared);

		// Analyze port_value against decl.
		Analysis::Dimensions::DimList port_value_dims;
		Analysis::Dimensions::analyze_expr(port_value, m_dim_map, port_value_dims);
		if (!port_value_dims.is_fully_packed()) {
			continue;
		}

		// Generate the declaration
		auto itdecl = module_dim_map.find(port->get_name());
		if (itdecl == module_dim_map.end()) {
			LOG_ERROR_N(port) << "port not found in module definition";
			return 1;
		}

		// copy the dims in order to be able to append instance array dim
		auto decl_dims = itdecl->second;
		if (!decl_dims.is_fully_packed()) {
			LOG_ERROR_N(port) << "port in module definition is not fully packed";
			return 1;
		}

		if ((decl_dims.packed_width() * array_dim.width) == port_value_dims.packed_width()) {
			decl_dims.list.push_front(array_dim);
		}

		const auto &decl = Analysis::Dimensions::generate_decl(id_str, AST::NodeType::Wire, decl_dims,
		                                                       port->get_filename(), port->get_line());
		new_stmts->push_back(decl);

		// update the dimensions map
		m_dim_map.emplace(id_str, decl_dims);

		// Replace the port value by the identifier
		const auto &identifier = std::make_shared<AST::Identifier>(nullptr, id_str,
		                                                           port->get_filename(), port->get_line());
		port->set_value(identifier);

		// Prepare the assignation.
		AST::Rvalue::Ptr rvalue;
		AST::Lvalue::Ptr lvalue;
		auto it_out_search = std::find(module_decl_outputs.cbegin(), module_decl_outputs.cend(), port->get_name());
		if (it_out_search == module_decl_outputs.cend()) {
			rvalue = std::make_shared<AST::Rvalue>(port_value, port->get_filename(), port->get_line());
			lvalue = std::make_shared<AST::Lvalue>(identifier, port->get_filename(), port->get_line());
		}
		else {
			rvalue = std::make_shared<AST::Rvalue>(identifier, port->get_filename(), port->get_line());
			lvalue = std::make_shared<AST::Lvalue>(port_value, port->get_filename(), port->get_line());
		}

		// Convert concat in lvalue
		ret = convert_to_lconcat(lvalue->get_var(), lvalue);
		if (ret != 0) {
			LOG_ERROR_N(lvalue) << "cannot convert concat to lvalue";
			return 1;
		}

		// Create the assignation.
		const auto &assign = std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr,
		                                                   port->get_filename(), port->get_line());
		new_stmts->push_back(assign);
	}

	new_stmts->push_back(instancelist);
	parent->replace(instancelist, new_stmts);

	return 0;
}



}
}
}
