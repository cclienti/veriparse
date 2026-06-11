// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>
#include <algorithm>
#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

AST::Arg::ListPtr Function::get_ioport_nodes(AST::Node::Ptr node) { return get_iodir_nodes(node); }

AST::Arg::ListPtr Function::get_iodir_nodes(AST::Node::Ptr node)
{
    AST::Arg::ListPtr list = std::make_shared<AST::Arg::List>();
    get_node_list<AST::Arg>(node, AST::NodeType::Arg, list);
    return list;
}

std::vector<std::string> Function::get_iodir_names(AST::Node::Ptr node)
{
    AST::Arg::ListPtr iodirs = get_iodir_nodes(node);
    return get_property_in_list<std::string, AST::Arg>(
        iodirs, [](AST::Arg::Ptr n) { return n->get_name(); });
}

AST::Declaration::ListPtr Function::get_variable_nodes(AST::Node::Ptr node)
{
    // A function's local variables are the Var/Net declared in its body — NOT its
    // arguments (those are Arg, via get_iodir_nodes) and NOT its return type.
    AST::Declaration::ListPtr list = std::make_shared<AST::Declaration::List>();

    // A bare directional placeholder (`input value` -> ImplicitNet + ImplicitType)
    // is not a real variable declaration; only Var/Net with a concrete type are.
    auto is_declared_signal = [](const AST::Declaration::Ptr &d) {
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
    };

    auto collect = [&](AST::Node::Ptr root) {
        AST::Declaration::ListPtr decls = std::make_shared<AST::Declaration::List>();
        get_node_list_by_category<AST::Declaration>(root, AST::NodeType::Declaration, decls);
        for(const AST::Declaration::Ptr &d : *decls) {
            if(is_declared_signal(d)) {
                list->push_back(d);
            }
        }
    };

    if(node && node->is_node_type(AST::NodeType::Function)) {
        const auto func = AST::cast_to<AST::Function>(node);
        if(func->get_statements()) {
            for(const AST::Node::Ptr &s : *func->get_statements()) {
                collect(s);
            }
        }
    } else {
        collect(node);
    }

    return list;
}

std::vector<std::string> Function::get_variable_names(AST::Node::Ptr node)
{
    AST::Declaration::ListPtr variables = get_variable_nodes(node);
    return get_property_in_list<std::string, AST::Declaration>(
        variables, [](AST::Declaration::Ptr n) { return n->get_name(); });
}

bool Function::is_like_automatic(const AST::Function::Ptr &node)
{
    if(node->get_lifetime() == AST::Function::LifetimeEnum::AUTOMATIC) {
        return true;
    }

    std::set<std::string> io_set;
    std::set<std::string> var_set;

    for(const auto &var : Function::get_variable_names(node)) {
        var_set.emplace(var);
    }

    if(var_set.empty()) {
        return true;
    }

    const auto iodirs = Function::get_iodir_nodes(node);
    for(const auto &io : *iodirs) {
        if(io->get_direction() == AST::Arg::DirectionEnum::INPUT) {
            io_set.emplace(io->get_name());
        } else {
            return false;
        }
    }

    if(var_set == io_set) {
        return true;
    }

    return false;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
