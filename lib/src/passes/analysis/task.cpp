// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/task.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

AST::Arg::ListPtr Task::get_iodir_nodes(AST::Node::Ptr node)
{
    AST::Arg::ListPtr list = std::make_shared<AST::Arg::List>();
    get_node_list<AST::Arg>(node, AST::NodeType::Arg, list);
    return list;
}

std::vector<std::string> Task::get_iodir_names(AST::Node::Ptr node)
{
    AST::Arg::ListPtr iodirs = get_iodir_nodes(node);
    return get_property_in_list<std::string, AST::Arg>(
        iodirs, [](AST::Arg::Ptr n) { return n->get_name(); });
}

AST::Declaration::ListPtr Task::get_variable_nodes(AST::Node::Ptr node)
{
    AST::Declaration::ListPtr decls = std::make_shared<AST::Declaration::List>();
    get_node_list_by_category<AST::Declaration>(node, AST::NodeType::Declaration, decls);
    AST::Declaration::ListPtr list = std::make_shared<AST::Declaration::List>();
    for(const AST::Declaration::Ptr &d : *decls) {
        // Skip bare directional placeholders (ImplicitNet + ImplicitType); only
        // Var/Net with a concrete type are real variable declarations.
        if(!(d->is_node_type(AST::NodeType::Var) || d->is_node_category(AST::NodeType::Net))) {
            continue;
        }
        if(d->is_node_type(AST::NodeType::ImplicitNet)) {
            const AST::DataType::Ptr type = d->get_type();
            if(!type || type->is_node_type(AST::NodeType::ImplicitType)) {
                continue;
            }
        }
        list->push_back(d);
    }
    return list;
}

std::vector<std::string> Task::get_variable_names(AST::Node::Ptr node)
{
    AST::Declaration::ListPtr variables = get_variable_nodes(node);
    return get_property_in_list<std::string, AST::Declaration>(
        variables, [](AST::Declaration::Ptr n) { return n->get_name(); });
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
