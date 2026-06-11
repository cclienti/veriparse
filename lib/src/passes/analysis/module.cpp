// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

int Module::get_module_dictionary(const AST::Node::ListPtr &node_list, ModulesMap &modules_map)
{
    if(!node_list) {
        return 1;
    }

    for(const auto &node : *node_list) {
        if(get_module_dictionary(node, modules_map)) {
            return 1;
        }
    }

    return 0;
}

int Module::get_module_dictionary(const AST::Node::Ptr &node, ModulesMap &modules_map)
{
    if(!node) {
        return 1;
    }

    const auto &modules = get_module_nodes(node);
    for(const auto &module : *modules) {
        if(modules_map.count(module->get_name()) != 0) {
            LOG_ERROR_N(module) << "module " << module->get_name() << " already declared";
            LOG_ERROR_N(modules_map[module->get_name()])
                << "module " << module->get_name() << " was firstly found here";
            return 1;
        }
        modules_map.emplace(module->get_name(), module);
    }
    return 0;
}

AST::Node::ListPtr Module::get_port_nodes(AST::Node::Ptr node)
{
    const auto &modules = get_module_nodes(node);
    if(modules->size() != 1) {
        LOG_ERROR_N(node) << "cannot analyze module port names, multiple modules found in the AST";
        return nullptr;
    }

    const auto &module = modules->front();
    const AST::Port::ListPtr ports = module->get_ports();

    AST::Node::ListPtr list = std::make_shared<AST::Node::List>();
    if(ports) {
        for(const AST::Port::Ptr &p : *ports) {
            list->push_back(p);
        }
    }
    return list;
}

namespace
{
// A declared signal is a Var or any Net — but NOT a bare port placeholder. A
// non-ANSI directional declaration with neither a net nor a data type keyword
// (`input clock`, `input [7:0] x`) parses as an ImplicitNet carrying an
// ImplicitType; its real signal is declared separately (`wire clock`) or defaults
// to a net, so it is not itself a variable declaration. An ImplicitNet with a
// concrete data type (`output reg valid` -> ImplicitNet + RegType) IS a real
// declaration and is kept.
bool is_declared_signal(const AST::Declaration::Ptr &d)
{
    if(!(d->is_node_type(AST::NodeType::Var) || d->is_node_category(AST::NodeType::Net))) {
        return false;
    }
    if(d->is_node_type(AST::NodeType::ImplicitNet)) {
        const AST::DataType::Ptr type = d->get_type();
        if(!type || type->is_node_type(AST::NodeType::ImplicitType)) {
            return false;
        }
    }
    return true;
}

// Effective signal name of a port: the port's own name (non-ANSI / explicit)
// or, for an ANSI typed port, the name carried by its inner declaration.
std::string port_name(const AST::Port::Ptr &p)
{
    if(!p->get_name().empty()) {
        return p->get_name();
    }
    if(p->get_decl()) {
        return p->get_decl()->get_name();
    }
    return std::string();
}
} // namespace

std::vector<std::string> Module::get_port_names(AST::Node::Ptr node)
{
    const auto &nodes = get_port_nodes(node);
    if(!nodes) {
        return std::vector<std::string>();
    }
    std::vector<std::string> names;
    for(const auto &item : *nodes) {
        if(item && item->is_node_type(AST::NodeType::Port)) {
            names.push_back(port_name(AST::cast_to<AST::Port>(item)));
        }
    }
    return names;
}

AST::Module::ListPtr Module::get_module_nodes(AST::Node::Ptr node)
{
    AST::Module::ListPtr list = std::make_shared<AST::Module::List>();
    get_node_list<AST::Module>(node, AST::NodeType::Module, list);
    return list;
}

std::vector<std::string> Module::get_module_names(AST::Node::Ptr node)
{
    AST::Module::ListPtr modules = get_module_nodes(node);
    return get_property_in_list<std::string, AST::Module>(
        modules, [](AST::Module::Ptr n) { return n->get_name(); });
}

AST::Param::ListPtr Module::get_parameter_nodes(AST::Node::Ptr node)
{
    AST::Param::ListPtr all = std::make_shared<AST::Param::List>();
    get_node_list<AST::Param>(node, AST::NodeType::Param, all);
    AST::Param::ListPtr list = std::make_shared<AST::Param::List>();
    for(const AST::Param::Ptr &p : *all) {
        if(!p->get_is_local()) {
            list->push_back(p);
        }
    }
    return list;
}

std::vector<std::string> Module::get_parameter_names(AST::Node::Ptr node)
{
    AST::Param::ListPtr parameters = get_parameter_nodes(node);
    return get_property_in_list<std::string, AST::Param>(
        parameters, [](AST::Param::Ptr n) { return n->get_name(); });
}

AST::Param::ListPtr Module::get_localparam_nodes(AST::Node::Ptr node)
{
    AST::Param::ListPtr all = std::make_shared<AST::Param::List>();
    get_node_list<AST::Param>(node, AST::NodeType::Param, all);
    AST::Param::ListPtr list = std::make_shared<AST::Param::List>();
    for(const AST::Param::Ptr &p : *all) {
        if(p->get_is_local()) {
            list->push_back(p);
        }
    }
    return list;
}

std::vector<std::string> Module::get_localparam_names(AST::Node::Ptr node)
{
    AST::Param::ListPtr localparams = get_localparam_nodes(node);
    return get_property_in_list<std::string, AST::Param>(
        localparams, [](AST::Param::Ptr n) { return n->get_name(); });
}

int Module::get_function_dictionary(const AST::Node::ListPtr &node_list, FunctionMap &function_map)
{
    if(!node_list) {
        return 1;
    }

    for(const auto &node : *node_list) {
        if(get_function_dictionary(node, function_map)) {
            return 1;
        }
    }

    return 0;
}

int Module::get_function_dictionary(const AST::Node::Ptr &node, FunctionMap &function_map)
{
    if(!node) {
        return 0;
    }

    const auto &functions = get_function_nodes(node);
    for(const auto &function : *functions) {
        const auto &fct_name = function->get_name();
        if(function_map.count(fct_name) != 0) {
            LOG_ERROR_N(function) << "function " << fct_name << " already declared";
            LOG_ERROR_N(function_map[fct_name])
                << "function " << fct_name << " was firstly found here";
            return 1;
        }
        function_map.emplace(fct_name, function);
    }

    return 0;
}

AST::Function::ListPtr Module::get_function_nodes(AST::Node::Ptr node)
{
    AST::Function::ListPtr list = std::make_shared<AST::Function::List>();
    get_node_list<AST::Function>(node, AST::NodeType::Function, list);
    return list;
}

std::vector<std::string> Module::get_function_names(AST::Node::Ptr node)
{
    AST::Function::ListPtr functions = get_function_nodes(node);
    return get_property_in_list<std::string, AST::Function>(
        functions, [](AST::Function::Ptr n) { return n->get_name(); });
}

AST::Task::ListPtr Module::get_task_nodes(AST::Node::Ptr node)
{
    AST::Task::ListPtr list = std::make_shared<AST::Task::List>();
    get_node_list<AST::Task>(node, AST::NodeType::Task, list);
    return list;
}

std::vector<std::string> Module::get_task_names(AST::Node::Ptr node)
{
    AST::Task::ListPtr tasks = get_task_nodes(node);
    return get_property_in_list<std::string, AST::Task>(
        tasks, [](AST::Task::Ptr n) { return n->get_name(); });
}

AST::TaskCall::ListPtr Module::get_taskcall_nodes(AST::Node::Ptr node)
{
    AST::TaskCall::ListPtr list = std::make_shared<AST::TaskCall::List>();
    get_node_list<AST::TaskCall>(node, AST::NodeType::TaskCall, list);
    return list;
}

std::vector<std::string> Module::get_taskcall_names(AST::Node::Ptr node)
{
    AST::TaskCall::ListPtr taskcalls = get_taskcall_nodes(node);
    return get_property_in_list<std::string, AST::TaskCall>(
        taskcalls, [](AST::TaskCall::Ptr n) { return n->get_name(); });
}

AST::SystemCall::ListPtr Module::get_systemcall_nodes(AST::Node::Ptr node)
{
    AST::SystemCall::ListPtr list = std::make_shared<AST::SystemCall::List>();
    get_node_list<AST::SystemCall>(node, AST::NodeType::SystemCall, list);
    return list;
}

std::vector<std::string> Module::get_systemcall_names(AST::Node::Ptr node)
{
    AST::SystemCall::ListPtr systemcalls = get_systemcall_nodes(node);
    return get_property_in_list<std::string, AST::SystemCall>(
        systemcalls, [](AST::SystemCall::Ptr n) { return n->get_syscall(); });
}

AST::FunctionCall::ListPtr Module::get_functioncall_nodes(AST::Node::Ptr node)
{
    AST::FunctionCall::ListPtr list = std::make_shared<AST::FunctionCall::List>();
    get_node_list<AST::FunctionCall>(node, AST::NodeType::FunctionCall, list);
    return list;
}

std::vector<std::string> Module::get_functioncall_names(AST::Node::Ptr node)
{
    AST::FunctionCall::ListPtr functioncalls = get_functioncall_nodes(node);
    return get_property_in_list<std::string, AST::FunctionCall>(
        functioncalls, [](AST::FunctionCall::Ptr n) { return n->get_name(); });
}

AST::Lvalue::ListPtr Module::get_lvalue_nodes(AST::Node::Ptr node)
{
    AST::Lvalue::ListPtr list = std::make_shared<AST::Lvalue::List>();
    get_node_list<AST::Lvalue>(node, AST::NodeType::Lvalue, list);
    return list;
}

AST::Rvalue::ListPtr Module::get_rvalue_nodes(AST::Node::Ptr node)
{
    AST::Rvalue::ListPtr list = std::make_shared<AST::Rvalue::List>();
    get_node_list<AST::Rvalue>(node, AST::NodeType::Rvalue, list);
    return list;
}

AST::Identifier::ListPtr Module::get_rvalue_identifier_nodes(AST::Node::Ptr node)
{
    AST::Rvalue::ListPtr rvalues = Module::get_rvalue_nodes(node);
    AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
    for(AST::Rvalue::Ptr rvalue : *rvalues) {
        get_node_list<AST::Identifier>(rvalue, AST::NodeType::Identifier, list);
    }
    return list;
}

std::vector<std::string> Module::get_rvalue_identifier_names(AST::Node::Ptr node)
{
    AST::Identifier::ListPtr identifiers = get_rvalue_identifier_nodes(node);
    return get_property_in_list<std::string, AST::Identifier>(
        identifiers, [](AST::Identifier::Ptr n) { return n->get_name(); });
}

AST::Port::ListPtr Module::get_iodir_nodes(AST::Node::Ptr node)
{
    // The direction-carrying ports (ANSI header ports, or non-ANSI body
    // direction declarations). Non-ANSI header references (direction NONE) are
    // name-only placeholders and are excluded — they are not IO directions.
    AST::Port::ListPtr all = std::make_shared<AST::Port::List>();
    get_node_list<AST::Port>(node, AST::NodeType::Port, all);
    AST::Port::ListPtr list = std::make_shared<AST::Port::List>();
    for(const AST::Port::Ptr &p : *all) {
        if(p->get_direction() != AST::Port::DirectionEnum::NONE) {
            list->push_back(p);
        }
    }
    return list;
}

std::vector<std::string> Module::get_iodir_names(AST::Node::Ptr node)
{
    AST::Port::ListPtr iodirs = get_iodir_nodes(node);
    return get_property_in_list<std::string, AST::Port>(
        iodirs, [](AST::Port::Ptr n) { return port_name(n); });
}

namespace
{
AST::Port::ListPtr ports_by_direction(AST::Node::Ptr node, AST::Port::DirectionEnum dir)
{
    AST::Port::ListPtr all = Module::get_iodir_nodes(node);
    AST::Port::ListPtr list = std::make_shared<AST::Port::List>();
    for(const AST::Port::Ptr &p : *all) {
        if(p->get_direction() == dir) {
            list->push_back(p);
        }
    }
    return list;
}
} // namespace

AST::Port::ListPtr Module::get_output_nodes(AST::Node::Ptr node)
{
    return ports_by_direction(node, AST::Port::DirectionEnum::OUTPUT);
}

std::vector<std::string> Module::get_output_names(AST::Node::Ptr node)
{
    AST::Port::ListPtr outputs = get_output_nodes(node);
    return get_property_in_list<std::string, AST::Port>(
        outputs, [](AST::Port::Ptr n) { return port_name(n); });
}

AST::Port::ListPtr Module::get_inout_nodes(AST::Node::Ptr node)
{
    return ports_by_direction(node, AST::Port::DirectionEnum::INOUT);
}

std::vector<std::string> Module::get_inout_names(AST::Node::Ptr node)
{
    AST::Port::ListPtr inouts = get_inout_nodes(node);
    return get_property_in_list<std::string, AST::Port>(
        inouts, [](AST::Port::Ptr n) { return port_name(n); });
}

AST::Port::ListPtr Module::get_input_nodes(AST::Node::Ptr node)
{
    return ports_by_direction(node, AST::Port::DirectionEnum::INPUT);
}

std::vector<std::string> Module::get_input_names(AST::Node::Ptr node)
{
    AST::Port::ListPtr inputs = get_input_nodes(node);
    return get_property_in_list<std::string, AST::Port>(
        inputs, [](AST::Port::Ptr n) { return port_name(n); });
}

AST::Declaration::ListPtr Module::get_variable_nodes(AST::Node::Ptr node)
{
    // Declared signals: Var and the node-per-net-type Net hierarchy (not params,
    // ports, args, members or typedefs). Single category walk preserves order.
    AST::Declaration::ListPtr decls = std::make_shared<AST::Declaration::List>();
    get_node_list_by_category<AST::Declaration>(node, AST::NodeType::Declaration, decls);
    AST::Declaration::ListPtr list = std::make_shared<AST::Declaration::List>();
    for(const AST::Declaration::Ptr &d : *decls) {
        if(is_declared_signal(d)) {
            list->push_back(d);
        }
    }
    return list;
}

std::vector<std::string> Module::get_variable_names(AST::Node::Ptr node)
{
    AST::Declaration::ListPtr variables = get_variable_nodes(node);
    return get_property_in_list<std::string, AST::Declaration>(
        variables, [](AST::Declaration::Ptr n) { return n->get_name(); });
}

AST::Declaration::ListPtr Module::get_variable_nodes_within_module(AST::Node::Ptr node)
{
    AST::Declaration::ListPtr varlist = get_variable_nodes(node);
    AST::Declaration::ListPtr varlist_w = std::make_shared<AST::Declaration::List>();
    std::vector<std::string> varname_w = get_variable_names_within_module(node);

    for(AST::Declaration::Ptr var : *varlist) {
        const std::string name = var->get_name();
        if(std::find(varname_w.begin(), varname_w.end(), name) != varname_w.end()) {
            varlist_w->push_back(var);
        }
    }

    return varlist_w;
}

std::vector<std::string> Module::get_variable_names_within_module(AST::Node::Ptr node)
{
    std::vector<std::string> var_names = get_variable_names(node);
    std::vector<std::string> io_names = get_iodir_names(node);

    for(const std::string &name : io_names) {
        var_names.erase(std::remove(var_names.begin(), var_names.end(), name), var_names.end());
    }

    return var_names;
}

AST::Instance::ListPtr Module::get_instance_nodes(AST::Node::Ptr node)
{
    AST::Instance::ListPtr list = std::make_shared<AST::Instance::List>();
    get_node_list<AST::Instance>(node, AST::NodeType::Instance, list);
    return list;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
