#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/annotate_scope.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>

#include <boost/algorithm/string/join.hpp>

#include <algorithm>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class InstTreeNode: public Misc::TreeNode<std::pair<std::string, std::string>>
{
public:
	InstTreeNode(const std::string &module, const std::string &instance):
		TreeNode(std::make_pair(module, instance))
	{}

	/**
	 * @brief Check if the scoped identifier corresponds to an instance
	 * hierarchy.
	 */
	bool match_scope(const AST::Identifier::Ptr &identifier, std::string &matched) const
	{
		auto scope = identifier->get_scope();
		if (!scope) {
			return false;
		}

		auto labellist = scope->get_labellist();
		if (!labellist) {
			return false;
		}

		// Start to look for the scope in the first tree node
		const InstTreeNode *node = this;

		// For each scope, go down in the tree to mactch with an instance
		for (auto labelit = labellist->cbegin(); labelit != labellist->cend();) {
			bool found = false;

			// Check each tree child if it matches with the scope
			for (const auto &child: node->get_children()) {
				const auto &labelscope = (*labelit)->get_scope();
				if (child->get_value().second == labelscope) {
					// We found the right instance that matches the scope
					found = true;
					// Increment the iterator to check the next label
					++labelit;
					// The child becomes the new node
					node = static_cast<const InstTreeNode*>(child.get());
					// Update the string that will be used to replace the
					// scoped identifier.
					if (!matched.empty()) {
						matched += std::string("_");
					}
					matched += labelscope;
					// Analyse the new node
					break;
				}
			}

			// Return if it does not match
			if (!found) {
				return false;
			}
		}

		return true;
	}

private:
	/**
	 * @brief return the string "module(instance)".
	 */
	std::string print_value() const final
	{
		return get_value().first + "(" + get_value().second + ")";
	}

	/**
	 * @brief allocate a new InstTreeNode unique pointer.
	 */
	Ptr make_ptr(const std::pair<std::string, std::string> &value) const final
	{
		return std::make_unique<InstTreeNode>(value.first, value.second);
	}
};


ModuleFlattener::ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
                                 const Analysis::Module::ModulesMap &modules_map):
	m_paramlist_inst (paramlist_inst),
	m_modules_map (modules_map)
{
}

ModuleFlattener::~ModuleFlattener()
{
}

ModuleFlattener::TreeNode::Ptr ModuleFlattener::get_instance_tree() const
{
	if (m_instance_tree) {
		return m_instance_tree->clone();
	}

	return nullptr;
}

int ModuleFlattener::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
	// Resolve the module
	ResolveModule resolver(m_paramlist_inst, m_modules_map);
	if (resolver.run(node)) {
		LOG_ERROR_N(node) << "failed to resolve the module";
		return 1;
	}

	// Collect all variable types
	const auto &vars = Analysis::Module::get_variable_nodes(node);
	for (const auto &var: *vars) {
		m_var_type_map.emplace(var->get_name(), var->get_node_type());
	}

	// Analysis declarations
	if (Analysis::UniqueDeclaration::analyze(node, m_declared)) {
		LOG_ERROR_N(node) << "failed to analyze declarations";
		return 1;
	}

	// Extract defparams
	if (extract_defparam(node, nullptr)) {
		LOG_ERROR_N(node) << "failed to manage defparams";
		return 1;
	}

	// Flatten
	if (flattener(node, parent)) {
		LOG_ERROR_N(node) << "failed to inline the module";
		return 1;
	}

	// Restore defparams
	if (restore_defparam(node)) {
		LOG_ERROR_N(node) << "failed to restore defparams";
		return 1;
	}

	if (replace_scoped_identifiers(node)) {
		LOG_ERROR_N(node) << "failed to replace identifiers that correspond to flattened instance hierarchies";
		return 1;
	}

	return 0;
}

int ModuleFlattener::flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	using VGen = Veriparse::Generators::VerilogGenerator;

	if (!node) {
		return 0;
	}

	switch (node->get_node_type()) {
	case AST::NodeType::Instancelist:
		{
			const auto &instancelist = AST::cast_to<AST::Instancelist>(node);
			const auto &instances = instancelist->get_instances();
			if (instances->size() != 1) {
				LOG_ERROR_N(instancelist) << "module not properly resolved";
				return 1;
			}

			const auto &instance = instances->front();
			const std::string &module_name = instance->get_module();
			const std::string &instance_name = instance->get_name();

			auto it_module = m_modules_map.find(module_name);
			if (it_module == m_modules_map.end()) {
				LOG_INFO_N(node) << "keeping " << instance_name << " (" << module_name << ")";
				return 0;
			}
			auto module = it_module->second->clone();

			LOG_INFO_N(node) << "flattenning " << instance_name << " (" << module_name << ")";

			// Manage defparam.
			//
			// Get all defparams that match the instance name. Remove
			// them from the current instance and append them into
			// 'module' (remove buffer the instance name in the defparam
			// name).
			//
			const auto &defparam_module = AST::cast_to<AST::Module>(module);
			auto module_items = defparam_module->get_items();
			if (!module_items) {
				module_items = std::make_shared<AST::Node::List>();
				defparam_module->set_items(module_items);
			}

			auto defparams_range = m_defparams.equal_range(instance_name);
			for (auto it = defparams_range.first; it != defparams_range.second; it++) {
				LOG_INFO_N(node) << "applying defparam " << it->first << " to " << module_name;
				const auto &defp = it->second->get_list()->front();
				defp->get_identifier()->get_scope()->get_labellist()->pop_front();
				module_items->push_back(it->second);
			}

			m_defparams.erase(defparams_range.first, defparams_range.second);

			// Check parameters declared in the instance against
			// declared in the module.
			auto paramlist_module = Analysis::Module::get_parameter_nodes(module);
			auto paramlist_inst = instance->get_parameterlist();
			if (paramlist_inst) {
				for (auto &param_inst: *paramlist_inst) {
					// if a value is declared, we continue.
					if (param_inst->get_value()) {
						LOG_INFO_N(node) << instance_name << " (" << module_name << "), "
						                 << "parameter " << param_inst->get_name() << ", "
						                 << "using instanciated value: ("
						                 << VGen().render(param_inst->get_value()) << ")";
						continue;
					}

					// Check that paremeters exist in the module
					if (!paramlist_module) {
						LOG_ERROR_N(node) << "no parameter declared in module " << module_name;
						return 1;
					}

					bool found = false;
					for (auto &param_module: *paramlist_module) {
						if (param_module->get_name() == param_inst->get_name()) {
							found = true;
							auto default_value = param_module->get_value();
							if (!default_value) {
								LOG_ERROR_N(param_module) << "no default value declared";
								return 1;
							}
							LOG_INFO_N(node) << instance_name << " (" << module_name << "), "
							                 << "parameter " << param_inst->get_name() << ", "
							                 << "using default value from module definition: "
							                 << VGen().render(default_value) << ")";
						}
					}

					if (!found) {
						LOG_ERROR_N(node) << "parameter " << param_inst->get_name()
						                  << "not found in module " << module_name;
						return 1;
					}
				}
			}

			// Filter null value in paramlist_inst
			if (paramlist_inst) {
				paramlist_inst->remove_if([](const AST::ParamArg::Ptr &p){return p->get_value() == nullptr;});
			}

			// Resolve the instantiated module
			ModuleFlattener flattener(paramlist_inst, m_modules_map);
			if (flattener.run(module)) {
				LOG_ERROR_N(node) << "failed to flatten the module";
				return 1;
			}

			// Construct the (module, instance) tree
			auto current_tree_node = std::make_unique<InstTreeNode>(module_name, instance_name);

			auto flat_tree_node = flattener.get_instance_tree();
			if (flat_tree_node) {
				auto &children = flat_tree_node->get_children();
				for (auto it = children.begin(); it != children.end(); ++it) {
					current_tree_node->push_child(std::move(*it));
				}
			}

			if (m_instance_tree) {
				m_instance_tree->push_child(std::move(current_tree_node));
			}
			else {
				LOG_ERROR_N(node) << "No module declaration found in the file";
				return 1;
			}

			// Check if there is some remaining unresolved parameter.
			const auto &remaining_parameters = Analysis::Module::get_parameter_names(module);
			if (!remaining_parameters.empty()) {
				std::string joined = boost::algorithm::join(remaining_parameters, ", ");
				LOG_ERROR_N(node) << "non resolved parameter found for "
				                  << instance_name << " (" << module_name << "): " << joined;
				return 1;
			}

			// Prefix content of the module
			AnnotateDeclaration decl_prefixer("^.*$", instance_name + "_$&", false);
			if (decl_prefixer.run(module)) {
				LOG_ERROR_N(node) << "cannot prefix module instance";
				return 1;
			}

			AnnotateScope scope_prefixer("^.*$", instance_name + "_$&");
			if (scope_prefixer.run(module)) {
				LOG_ERROR_N(node) << "cannot prefix module instance";
				return 1;
			}

			// Bind module
			const auto &mod_cast = AST::cast_to<AST::Module>(module);
			auto binded_items = bind(instance, mod_cast);
			if (!binded_items) {
				LOG_ERROR_N(node) << "Could not bind " << instance_name << " (" << module_name << ")";
				return 1;
			}

			// Analyze module's declarations
			if (Analysis::UniqueDeclaration::analyze(module, m_declared)) {
				LOG_ERROR_N(module) << "failed to analyze declarations";
				return 1;
			}

			// Append module to binded items and replace the instance
			const auto &mod_items = mod_cast->get_items();
			if (mod_items) {
				binded_items->insert(binded_items->end(), mod_items->begin(), mod_items->end());
			}

			parent->replace(node, binded_items);
		}
		break;


	case AST::NodeType::Module:
		{
			const auto &module = AST::cast_to<AST::Module>(node);
			m_instance_tree = std::make_unique<InstTreeNode>(module->get_name(), "");
		}
		// We do not break here to recurse in the module;

	default:
		{
			int ret = 0;
			AST::Node::ListPtr children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += flattener(child, node);
			}
			return ret;
		}

	}

	return 0;
}

AST::Node::ListPtr ModuleFlattener::bind(const AST::Instance::Ptr &instance, const AST::Module::Ptr &module)
{
	const auto &ports = module->get_ports();
	if (!ports) {
		// Nothing to bind
		return std::make_shared<AST::Node::List>();
	}

	// Check consistency
	const auto &inst_ports = instance->get_portlist();
	if (!inst_ports) {
		LOG_ERROR_N(instance) << "module IO and instantiated ports are not consistent";
		return nullptr;
	}

	const auto &decl_items = std::make_shared<AST::Node::List>();
	const auto &assign_items = std::make_shared<AST::Node::List>();

	// Generate
	for (const auto &port: *ports) {
		const auto &ioport = AST::cast_to<AST::Ioport>(port);
		const auto &iodir = ioport->get_first();
		const auto &var = ioport->get_second();

		// Don't take ref here because variable can be updated with a
		// new name and we need the original name for the matching.
		const auto var_name = var->get_name();

		// Search for an unique identifier
		LOG_DEBUG_N(var) << "looking for '" << var_name << "' in declared variables";
		const std::string &var_new_name =
			Analysis::UniqueDeclaration::get_unique_identifier(var->get_name(), m_declared);

		// Renamed the cloned declaration
		if (var_new_name != var_name) {
			LOG_DEBUG_N(var) << "renaming '" << var_name << "' to '" << var_new_name << "'";
			AnnotateDeclaration renamer(var_name, var_new_name, false);
			// renaming variable in the module will also rename "var"
			renamer.run(module);
		}

		// Append declaration
		const auto &decl_var = var->clone();
		decl_items->push_back(decl_var);

		// Generate assignments
		AST::Node::Ptr inst_value = nullptr;
		for (const auto &inst_port: *inst_ports) {
			std::string port_name = instance->get_name() + "_" + inst_port->get_name();
			if (var_name == port_name) {
				inst_value = inst_port->get_value();
				LOG_DEBUG << var_new_name << " matched";
				break;
			}
		}

		if (!inst_value) {
			if (iodir->is_node_type(AST::NodeType::Input)) {
				LOG_WARNING << instance->get_name() << " (" << module->get_name() << ") " << "::"
				            << var_name << ", "
				            << "input left unconnected";
				return nullptr;
			}
			continue;
		}

		AST::Lvalue::Ptr lvalue;
		AST::Rvalue::Ptr rvalue;

		switch (iodir->get_node_type()) {
		case AST::NodeType::Input:
			lvalue = std::make_shared<AST::Lvalue>(std::make_shared<AST::Identifier>(nullptr, var_new_name));
			rvalue = std::make_shared<AST::Rvalue>(inst_value->clone());

			if (var->is_node_category(AST::NodeType::Net)) {
				assign_items->push_back(std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr));
			}
			else {
				const auto &subst = std::make_shared<AST::BlockingSubstitution>(lvalue, rvalue, nullptr, nullptr);
				const auto &senslist = std::make_shared<AST::Sens::List>();
				senslist->push_back(std::make_shared<AST::Sens>(nullptr, AST::Sens::TypeEnum::ALL));
				assign_items->push_back
					(std::make_shared<AST::Always>(std::make_shared<AST::Senslist>(senslist), subst));
			}

			break;

		case AST::NodeType::Output:
			lvalue = std::make_shared<AST::Lvalue>(inst_value->clone());
			rvalue = std::make_shared<AST::Rvalue>(std::make_shared<AST::Identifier>(nullptr, var_new_name));
			if (check_output_rvalue_wire(lvalue)) {
				convert_concat_to_lconcat(lvalue->get_var(), lvalue);
				assign_items->push_back(std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr));
			}
			else {
				return nullptr;
			}
			break;

		case AST::NodeType::Inout:
			LOG_ERROR_N(iodir) << "Inout port not supported during flattening, giving up "
			                   << instance->get_name() << " (" << module->get_name() << ") ";
			return nullptr;

		default:
			LOG_FATAL_N(iodir) << "Unknown IO type";
			return nullptr;
		}
	}

	decl_items->insert(decl_items->end(), assign_items->begin(), assign_items->end());

	return decl_items;
}


bool ModuleFlattener::check_output_rvalue_wire(const AST::Node::Ptr &node)
{
	bool ret = true;

	if (!node) {
		return ret;
	}

	switch(node->get_node_type()) {
	case AST::NodeType::Identifier:
		{
			const auto &id = AST::cast_to<AST::Identifier>(node);
			const auto it = m_var_type_map.find(id->get_name());
			if (it != m_var_type_map.end()) {
				if (it->second == AST::NodeType::Wire) {
					ret = true;
				}
				else {
					LOG_ERROR_N(node) << "Identifier '" << id->get_name() << "' is not declared as a wire";
					ret = false;
				}
			}
			else {
				LOG_ERROR_N(node) << "Identifier '" << id->get_name() << "' not declared";
				ret = false;
			}
		}
		break;

	case AST::NodeType::Indirect:
	case AST::NodeType::Partselect:
	case AST::NodeType::PartselectIndexed:
	case AST::NodeType::PartselectPlusIndexed:
	case AST::NodeType::PartselectMinusIndexed:
	case AST::NodeType::Pointer:
		return check_output_rvalue_wire(AST::cast_to<AST::Indirect>(node)->get_var());

	case AST::NodeType::Repeat:
	case AST::NodeType::StringConst:
	case AST::NodeType::FloatConst:
	case AST::NodeType::IntConst:
	case AST::NodeType::IntConstN:
	case AST::NodeType::Real:
		LOG_ERROR << "Invalid expression in an output port";
		ret = false;
		break;

	default:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret &= check_output_rvalue_wire(child);
			}
		}
	}

	return ret;
}


int ModuleFlattener::convert_concat_to_lconcat(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	int ret = 0;

	if (!node) {
		return ret;
	}

	switch(node->get_node_type()) {
	case AST::NodeType::Concat:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += convert_concat_to_lconcat(child, node);
			}

			const auto &concat = AST::cast_to<AST::Concat>(node);
			const auto &lconcat = std::make_shared<AST::Lconcat>(concat->get_list());

			parent->replace(node, lconcat);
		}
		break;

	default:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += convert_concat_to_lconcat(child, node);
			}
		}
	}

	return ret;
}


int ModuleFlattener::extract_defparam(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
	int ret = 0;

	if (!node) {
		return ret;
	}

	switch (node->get_node_type()) {
	case AST::NodeType::Defparamlist:
		{
			const auto &defparaml = AST::cast_to<AST::Defparamlist>(node);
			for (const auto &defparam: *defparaml->get_list()) {
				const auto &defp = AST::cast_to<AST::Defparam>(defparam);
				const auto &defplist = std::make_shared<AST::Defparam::List>();
				const auto &new_defparaml = std::make_shared<AST::Defparamlist>();
				new_defparaml->set_list(defplist);
				defplist->push_back(defp);
				const auto &scope = defp->get_identifier()->get_scope()->get_labellist()->front()->get_scope();
				m_defparams.emplace(scope, new_defparaml);
			}
			parent->remove(node);
		}
		break;

	default:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += extract_defparam(child, node);
			}
		}
	}

	return ret;
}


int ModuleFlattener::restore_defparam(const AST::Node::Ptr &node)
{
	int ret = 0;

	if (!node) {
		return ret;
	}

	switch (node->get_node_type()) {
	case AST::NodeType::Module:
		{
			const auto &module = AST::cast_to<AST::Module>(node);

			auto items = module->get_items();
			if (!items) {
				items = std::make_shared<AST::Node::List>();
				module->set_items(items);
			}

			for(const auto &entry: m_defparams) {
				items->push_back(entry.second);
			}

			module->set_items(items);
		}
		break;

	default:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += restore_defparam(child);
			}
		}
	}

	return ret;
}


int ModuleFlattener::replace_scoped_identifiers(const AST::Node::Ptr &node)
{
	int ret = 0;

	if (!node) {
		return ret;
	}

	switch(node->get_node_type()) {
	case AST::NodeType::Identifier:
		{
			const auto &identifier = AST::cast_to<AST::Identifier>(node);

			const auto &scope = identifier->get_scope();
			if (!scope) {
				return 0;
			}

			auto labellist = scope->get_labellist();
			if (!labellist) {
				return 0;
			}

			if (!m_instance_tree) {
				return 0;
			}

			const auto &tree = static_cast<const InstTreeNode&>(*m_instance_tree);
			std::string matched;
			if(tree.match_scope(identifier, matched)) {
				identifier->set_scope(nullptr);
				matched += "_" + identifier->get_name();
				identifier->set_name(matched);
			}
		}
		break;

	default:
		{
			const auto &children = node->get_children();
			for (AST::Node::Ptr child: *children) {
				ret += replace_scoped_identifiers(child);
			}
		}
	}

	return ret;
}


}
}
}
