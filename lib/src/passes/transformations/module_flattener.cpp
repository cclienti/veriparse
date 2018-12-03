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
    ResolveModule resolver(m_paramlist_inst, m_modules_map);
    if (resolver.run(node)) {
        LOG_ERROR_N(node) << "failed to resolve the module";
        return 1;
    }

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
                LOG_INFO_N(node) << "keeping " << module_name << "::" << instance_name;
                return 0;
            }
            auto module = it_module->second->clone();

            LOG_INFO_N(node) << "flattenning " << module_name << "::" << instance_name;

            // Check parameters declared in the instance against
            // declared in the module.
            auto paramlist_module = Analysis::Module::get_parameter_nodes(module);
            auto paramlist_inst = instance->get_parameterlist();
            if (paramlist_inst) {
                for (auto &param_inst: *paramlist_inst) {
                    // if a value is declared, we continue.
                    if (param_inst->get_value()) {
                        LOG_INFO_N(node) << module_name << "::" << instance_name << ", "
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
                            LOG_INFO_N(node) << module_name << "::" << instance_name << ", "
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
                                  << module_name << "::" << instance_name << ": " << joined;
                return 1;
            }

            // Prefix content of the module
            AnnotateDeclaration prefixer("^.*$", instance_name + "_$&", false);
            prefixer.run(module);

            // Bind module
            const auto &mod_cast = AST::cast_to<AST::Module>(module);
            auto binded_items = bind(instance, mod_cast);
            if (!binded_items) {
                LOG_ERROR_N(node) << "Could not bind " << module_name << "::" << instance_name;
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
    const auto &items = std::make_shared<AST::Node::List>();

    const auto &ports = module->get_ports();
    if (!ports) {
        // Nothing to bind
        return items;
    }

    // Check consistency
    const auto &inst_ports = instance->get_portlist();
    if (!inst_ports) {
        LOG_ERROR_N(instance) << "module IO and instantiated ports are not consistent";
        return nullptr;
    }

    // Generate declarations
    for (const auto &port: *ports) {
        if (!port->is_node_type(AST::NodeType::Ioport)) {
            LOG_FATAL_N(port) << "module IO not properly normalized";
            return nullptr;
        }
        const auto &var = AST::cast_to<AST::Ioport>(port)->get_second();
        items->push_back(var->clone());
    }

    // Generate assignments
    for (const auto &port: *ports) {
        const auto &ioport = AST::cast_to<AST::Ioport>(port);
        const auto &iodir = ioport->get_first();
        const auto &var = ioport->get_second();

        AST::Node::Ptr inst_value = nullptr;
        for (const auto &inst_port: *inst_ports) {
            std::string port_name = instance->get_name() + "_" + inst_port->get_name();
            if (var->get_name() == port_name) {
                inst_value = inst_port->get_value();
                LOG_DEBUG_N(inst_value) << var->get_name() << " matched";
                break;
            }
        }

        if (!inst_value) {
            if (iodir->is_node_type(AST::NodeType::Input)) {
                LOG_WARNING_N(inst_value) << module->get_name() << "::" << instance->get_name() << "::"
                                          << var->get_name() << ", "
                                          << "input left unconnected";
            }
            continue;
        }

        AST::Lvalue::Ptr lvalue;
        AST::Rvalue::Ptr rvalue;

        switch (iodir->get_node_type()) {
        case AST::NodeType::Input:
        case AST::NodeType::Inout:
            lvalue = std::make_shared<AST::Lvalue>(std::make_shared<AST::Identifier>(nullptr, var->get_name()));
            rvalue = std::make_shared<AST::Rvalue>(inst_value->clone());
            break;

        case AST::NodeType::Output:
            lvalue = std::make_shared<AST::Lvalue>(inst_value->clone());
            rvalue = std::make_shared<AST::Rvalue>(std::make_shared<AST::Identifier>(nullptr, var->get_name()));
            break;

        default:
            LOG_FATAL_N(iodir) << "Unknown IO type";
            return nullptr;
        }

        if (var->is_node_category(AST::NodeType::Net)) {
            items->push_back(std::make_shared<AST::Assign>(lvalue, rvalue, nullptr, nullptr));
        }
        else {
            const auto &subst = std::make_shared<AST::BlockingSubstitution>(lvalue, rvalue, nullptr, nullptr);
            const auto &senslist = std::make_shared<AST::Sens::List>();
            senslist->push_back(std::make_shared<AST::Sens>(nullptr, AST::Sens::TypeEnum::ALL));
            items->push_back(std::make_shared<AST::Always>(std::make_shared<AST::Senslist>(senslist), subst));
        }

    }

    return items;
}




}
}
}
