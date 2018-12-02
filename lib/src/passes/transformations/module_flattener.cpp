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

            // Resolve parameters
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
            paramlist_inst->remove_if([](const AST::ParamArg::Ptr &p){return p->get_value() == nullptr;});

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
            AnnotateDeclaration prefixer("^.*$", instance_name + "_$&");
            prefixer.run(module);

            // Bind module
            const auto &mod_cast = AST::cast_to<AST::Module>(module);
            auto binded_items = bind(mod_cast);
            if (!binded_items) {
                LOG_ERROR_N(node) << "Could not bind " << module_name << "::" << instance_name;
                return 1;
            }

            // Append module to binded items and replace the instance
            const auto &mod_items = mod_cast->get_items();
            if (mod_items) {
                binded_items->insert(binded_items->end(), mod_items->begin(), mod_items->end());
            }

            parent->replace(node, mod_items);
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

AST::Node::ListPtr ModuleFlattener::bind(const AST::Module::Ptr &module)
{
    const auto &items = std::make_shared<AST::Node::List>();

    return items;
}


// int ModuleFlattener::prefix_content(const std::string &prefix, ASTReplace::ReplaceMap &replaced_map,
//                                     const AST::Node::Ptr &node)
// {
//     if (!node) {
//         return 0;
//     }

//     int ret = 0;

//     if (node->is_node_category(AST::NodeType::VariableBase)) {
//         const auto &var = AST::cast_to<AST::VariableBase>(node);
//         var->set_name(prefix + "_" + var->get_name());
//     }
//     else if (node->is_node_type(AST::NodeType::Function)) {
//         const auto &function = AST::cast_to<AST::Function>(node);
//         function->set_name(prefix + "_" + function->get_name());
//         return 0; // We do not visit in function body
//     }
//     else if (node->is_node_type(AST::NodeType::FunctionCall)) {
//         const auto &function_call = AST::cast_to<AST::FunctionCall>(node);
//         function_call->set_name(prefix + "_" + function_call->get_name());
//     }
//     else if (node->is_node_type(AST::NodeType::Task)) {
//         const auto &task = AST::cast_to<AST::Task>(node);
//         task->set_name(prefix + "_" + task->get_name());
//         return 0; // We do not visit in task body
//     }
//     else if (node->is_node_type(AST::NodeType::TaskCall)) {
//         const auto &task_call = AST::cast_to<AST::TaskCall>(node);
//         task_call->set_name(prefix + "_" + task_call->get_name());
//     }
//     else if (node->is_node_type(AST::NodeType::Instance)) {
//         const auto &instance = AST::cast_to<AST::Instance>(node);
//         instance->set_name(prefix + "_" + instance->get_name());
//     }


//     const auto &children = node->get_children();
//     for (const auto &child: *children) {
//         ret += prefix_content(prefix, replaced_map, child);
//     }

//     return ret;
// }


}
}
}
