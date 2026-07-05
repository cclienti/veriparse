// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/scope_table.hpp>

#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

ScopeTable::SymbolKind ScopeTable::classify(const AST::Node::Ptr &decl)
{
    if(!decl) {
        return SymbolKind::UNKNOWN;
    }

    switch(decl->get_node_type()) {
    case AST::NodeType::Typedef:
    case AST::NodeType::TypeParam:
        return SymbolKind::TYPE;
    case AST::NodeType::Function:
        return SymbolKind::FUNCTION;
    case AST::NodeType::Task:
        return SymbolKind::TASK;
    case AST::NodeType::Module:
        return SymbolKind::MODULE;
    case AST::NodeType::Interface:
        return SymbolKind::INTERFACE;
    case AST::NodeType::Genvar:   // compile-time value, deliberately not a Declaration
    case AST::NodeType::EnumItem: // an enumerator is a constant in scope (§6.19)
        return SymbolKind::VALUE;
    default:
        // Var, the Net family, Param, Arg, Member, … — every remaining
        // Declaration binds a name to a value.
        if(decl->is_node_category(AST::NodeType::Declaration)) {
            return SymbolKind::VALUE;
        }
        return SymbolKind::UNKNOWN;
    }
}

void ScopeTable::for_each_bound_name(
    const AST::Node::Ptr &item,
    const std::function<void(const std::string &, const AST::Node::Ptr &)> &visit)
{
    if(!item) {
        return;
    }

    switch(item->get_node_type()) {
    case AST::NodeType::Function:
        visit(AST::cast_to<AST::Function>(item)->get_name(), item);
        return;

    case AST::NodeType::Task:
        visit(AST::cast_to<AST::Task>(item)->get_name(), item);
        return;

    case AST::NodeType::Genvar:
        visit(AST::cast_to<AST::Genvar>(item)->get_name(), item);
        return;

    default:
        break;
    }

    if(!item->is_node_category(AST::NodeType::Declaration)) {
        return;
    }

    const auto &decl = AST::cast_to<AST::Declaration>(item);
    if(!decl->get_name().empty()) {
        visit(decl->get_name(), item);
    }

    // An inline enum type also binds its enumerators in the enclosing scope
    // (§6.19).
    const auto &type = decl->get_type();
    if(type && type->is_node_type(AST::NodeType::EnumType)) {
        const auto &items = AST::cast_to<AST::EnumType>(type)->get_items();
        if(items) {
            for(const AST::EnumItem::Ptr &enum_item : *items) {
                visit(enum_item->get_name(), enum_item);
            }
        }
    }
}

void ScopeTable::add_local(const std::string &name, AST::Node::Ptr decl)
{
    m_local[name] = Binding{decl, "", Origin::Local, "", classify(decl)};
}

void ScopeTable::add_explicit_import(const std::string &name, const std::string &package,
                                     AST::Node::Ptr decl)
{
    m_explicit[name] = Binding{decl, package, Origin::ExplicitImport, package, classify(decl)};
}

void ScopeTable::add_wildcard_import(const std::string &name, const std::string &package,
                                     AST::Node::Ptr decl, const std::string &defining_package)
{
    m_wildcard[name].push_back(
        Binding{decl, package, Origin::WildcardImport, defining_package, classify(decl)});
}

const ScopeTable::Binding *ScopeTable::lookup(const std::string &name, bool *ambiguous) const
{
    if(ambiguous) {
        *ambiguous = false;
    }

    // §26.4–26.6 precedence: local > explicit import > wildcard import.
    auto local_it = m_local.find(name);
    if(local_it != m_local.end()) {
        return &local_it->second;
    }

    auto explicit_it = m_explicit.find(name);
    if(explicit_it != m_explicit.end()) {
        return &explicit_it->second;
    }

    auto wildcard_it = m_wildcard.find(name);
    if(wildcard_it != m_wildcard.end()) {
        // Several wildcard paths to one name conflict only when they reach
        // *different* declarations. Paths that resolve to the same originating
        // declaration (e.g. a package and another that re-exports it) are not a
        // conflict (§26.6), so dedup by the defining package.
        std::set<std::string> origins;
        for(const Binding &b : wildcard_it->second) {
            origins.insert(b.defining_package);
        }
        if(origins.size() == 1) {
            return &wildcard_it->second.front();
        }
        if(ambiguous) {
            *ambiguous = true;
        }
        return nullptr;
    }

    return nullptr;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
