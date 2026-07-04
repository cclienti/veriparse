// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

int NameResolution::run_design(const std::vector<AST::Node::Ptr> &sources)
{
    m_design.clear();
    m_scopes.clear();

    int ret = 0;
    for(const AST::Node::Ptr &source : sources) {
        ret += index_design(source);
    }
    if(ret) {
        return ret;
    }

    for(const AST::Node::Ptr &source : sources) {
        ret += resolve_source(source);
    }
    return ret;
}

int NameResolution::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    (void)parent;
    m_design.clear();
    m_scopes.clear();

    int ret = index_design(node);
    if(ret) {
        return ret;
    }
    return resolve_source(node);
}

int NameResolution::index_design(const AST::Node::Ptr &source)
{
    if(!source) {
        return 0;
    }

    switch(source->get_node_type()) {
    case AST::NodeType::Source:
        return index_design(AST::to_node(AST::cast_to<AST::Source>(source)->get_description()));

    case AST::NodeType::Description: {
        const auto &definitions = AST::cast_to<AST::Description>(source)->get_definitions();
        int ret = 0;
        if(definitions) {
            for(const AST::Node::Ptr &def : *definitions) {
                ret += index_design(def);
            }
        }
        return ret;
    }

    case AST::NodeType::Module:
    case AST::NodeType::Interface: {
        const ScopeTable::SymbolKind kind = ScopeTable::classify(source);
        std::string name;
        if(kind == ScopeTable::SymbolKind::MODULE) {
            name = AST::cast_to<AST::Module>(source)->get_name();
        } else {
            name = AST::cast_to<AST::Interface>(source)->get_name();
        }
        const auto it = m_design.find(name);
        if(it != m_design.end()) {
            LOG_ERROR_N(source) << "design element '" << name << "' is defined more than once";
            return 1;
        }
        m_design[name] = DesignEntry{kind, source};
        return 0;
    }

    default:
        return 0;
    }
}

int NameResolution::resolve_source(const AST::Node::Ptr &source) { return walk(source, nullptr); }

void NameResolution::add_scope_item(ScopeTable &table, const AST::Node::Ptr &item)
{
    if(!item) {
        return;
    }

    switch(item->get_node_type()) {
    case AST::NodeType::Function:
        table.add_local(AST::cast_to<AST::Function>(item)->get_name(), item);
        return;

    case AST::NodeType::Task:
        table.add_local(AST::cast_to<AST::Task>(item)->get_name(), item);
        return;

    case AST::NodeType::Genvar:
        table.add_local(AST::cast_to<AST::Genvar>(item)->get_name(), item);
        return;

    default:
        break;
    }

    if(item->is_node_category(AST::NodeType::Declaration)) {
        const auto &decl = AST::cast_to<AST::Declaration>(item);
        if(!decl->get_name().empty()) {
            table.add_local(decl->get_name(), item);
        }

        // An inline enum type also binds its enumerators in the enclosing
        // scope (§6.19).
        const auto &type = decl->get_type();
        if(type && type->is_node_type(AST::NodeType::EnumType)) {
            const auto &items = AST::cast_to<AST::EnumType>(type)->get_items();
            if(items) {
                for(const AST::EnumItem::Ptr &enum_item : *items) {
                    table.add_local(enum_item->get_name(), enum_item);
                }
            }
        }
    }
}

void NameResolution::add_scope_items(ScopeTable &table, const AST::Node::ListPtr &items)
{
    if(items) {
        for(const AST::Node::Ptr &item : *items) {
            add_scope_item(table, item);
        }
    }
}

const ScopeTable::Binding *NameResolution::lookup(const std::string &name) const
{
    for(auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        const ScopeTable::Binding *binding = it->lookup(name);
        if(binding) {
            return binding;
        }
    }
    return nullptr;
}

int NameResolution::walk_in_scope(const ScopeTable &table, const AST::Node::Ptr &node)
{
    m_scopes.push_back(table);

    int ret = 0;
    const AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        ret += walk(child, node);
    }

    m_scopes.pop_back();
    return ret;
}

int NameResolution::walk(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        ScopeTable table;
        if(module->get_params()) {
            for(const AST::Declaration::Ptr &param : *module->get_params()) {
                add_scope_item(table, param);
            }
        }
        if(module->get_ports()) {
            for(const AST::Port::Ptr &port : *module->get_ports()) {
                add_scope_item(table, port->get_decl());
            }
        }
        add_scope_items(table, module->get_items());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        ScopeTable table;
        if(interface->get_params()) {
            for(const AST::Declaration::Ptr &param : *interface->get_params()) {
                add_scope_item(table, param);
            }
        }
        if(interface->get_ports()) {
            for(const AST::Port::Ptr &port : *interface->get_ports()) {
                add_scope_item(table, port->get_decl());
            }
        }
        add_scope_items(table, interface->get_items());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Package: {
        ScopeTable table;
        add_scope_items(table, AST::cast_to<AST::Package>(node)->get_items());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Function: {
        const auto &function = AST::cast_to<AST::Function>(node);
        ScopeTable table;
        // The function's own name is bound inside its body (the legacy return
        // variable, and the callee of a recursive call).
        table.add_local(function->get_name(), node);
        if(function->get_args()) {
            for(const AST::Arg::Ptr &arg : *function->get_args()) {
                add_scope_item(table, arg);
            }
        }
        add_scope_items(table, function->get_statements());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Task: {
        const auto &task = AST::cast_to<AST::Task>(node);
        ScopeTable table;
        table.add_local(task->get_name(), node);
        if(task->get_args()) {
            for(const AST::Arg::Ptr &arg : *task->get_args()) {
                add_scope_item(table, arg);
            }
        }
        add_scope_items(table, task->get_statements());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Block: {
        ScopeTable table;
        add_scope_items(table, AST::cast_to<AST::Block>(node)->get_statements());
        return walk_in_scope(table, node);
    }

    case AST::NodeType::Call:
        // Exact type only: FunctionCall/TaskCall are already resolved forms.
        retag_call(AST::cast_to<AST::Call>(node), parent);
        return 0;

    default: {
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret += walk(child, node);
        }
        return ret;
    }
    }
}

void NameResolution::retag_call(const AST::Call::Ptr &call, const AST::Node::Ptr &parent)
{
    // Hierarchical calls (u1.t()) belong to elaboration, not to this pass.
    if(call->get_hier()) {
        return;
    }

    // A scoped call (pkg::t()) only reaches this pass un-inlined; its bare
    // name must not be looked up in the lexical stack (it could bind an
    // unrelated local), so it stays neutral.
    if(call->get_scope() && !call->get_scope()->empty()) {
        LOG_WARNING_N(call) << "scoped call '" << call->get_name()
                            << "' left unresolved (package scopes are resolved by "
                               "the package inliner)";
        return;
    }

    if(!parent) {
        return;
    }

    const ScopeTable::Binding *binding = lookup(call->get_name());
    if(!binding) {
        LOG_WARNING_N(call) << "unresolved statement call to '" << call->get_name() << "'";
        return;
    }

    switch(binding->kind) {
    case ScopeTable::SymbolKind::TASK: {
        auto taskcall = std::make_shared<AST::TaskCall>(call->get_filename(), call->get_line());
        taskcall->set_name(call->get_name());
        taskcall->set_args(call->get_args());
        parent->replace(call, taskcall);
        return;
    }

    case ScopeTable::SymbolKind::FUNCTION: {
        // Legal as a statement, but the return value is discarded — the
        // standard asks for a warning unless the call is void-cast (§13.4.1).
        LOG_WARNING_N(call) << "function '" << call->get_name()
                            << "' called as a statement; its return value is discarded";
        auto functioncall =
            std::make_shared<AST::FunctionCall>(call->get_filename(), call->get_line());
        functioncall->set_name(call->get_name());
        functioncall->set_args(call->get_args());
        parent->replace(call, functioncall);
        return;
    }

    default:
        LOG_WARNING_N(call) << "'" << call->get_name()
                            << "' is not a task or function; statement call left unresolved";
        return;
    }
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
