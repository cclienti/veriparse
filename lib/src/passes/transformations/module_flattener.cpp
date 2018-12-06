#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/analysis/instance.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>

#include <boost/algorithm/string/join.hpp>

#include <algorithm>


namespace Veriparse {
namespace Passes {
namespace Transformations {


ModuleFlattener::ModuleFlattener(const AST::ParamArg::ListPtr &paramlist_inst,
                                 const Analysis::Module::ModulesMap &modules_map):
    m_paramlist_inst (paramlist_inst),
	m_modules_map (modules_map)
{
}

ModuleFlattener::~ModuleFlattener()
{
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

    // Flatten
    if (flattener(node, parent)) {
        LOG_ERROR_N(node) << "failed to inline the module";
        return 1;
    }

	return 0;
}

int ModuleFlattener::flattener(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
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
                                         << "using instanciated value";
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
                            LOG_INFO_N(node) << instance_name << " (" << module_name << "), "
                                             << "parameter " << param_inst->get_name() << ", "
                                             << "using default value from module definition";
                            auto default_value = param_module->get_value();
                            if (!default_value) {
                                LOG_ERROR_N(param_module) << "no default value declared";
                                return 1;
                            }
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

            // Check if there is some remaining unresolved parameter.
            const auto &remaining_parameters = Analysis::Module::get_parameter_names(module);
            if (!remaining_parameters.empty()) {
                std::string joined = boost::algorithm::join(remaining_parameters, ", ");
                LOG_ERROR_N(node) << "non resolved parameter found for "
                                  << instance_name << " (" << module_name << "): " << joined;
                return 1;
            }

            // Prefix content of the module
            AnnotateDeclaration prefixer("^.*$", instance_name + "_$&", false);
            if (prefixer.run(module)) {
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

    case AST::NodeType::Repeat:
    case AST::NodeType::StringConst:
    case AST::NodeType::FloatConst:
    case AST::NodeType::IntConst:
    case AST::NodeType::IntConstN:
    case AST::NodeType::Real:
        LOG_ERROR_N(node) << "Invalid expression in an output port";
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

}
}
}
