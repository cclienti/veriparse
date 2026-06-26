// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

#include <list>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// A module/package opens a separate name scope; a scope is resolved on its own
/// traversal, so reference rewriting stops at these boundaries.
bool is_scope_boundary(const AST::Node::Ptr &node)
{
    return node->is_node_type(AST::NodeType::Module) || node->is_node_type(AST::NodeType::Package);
}

/// Name a package item declares, when it is a synthesizable declaration
/// (Param/Var/Net/Typedef via the Declaration category, or a Function/Task).
bool node_decl_name(const AST::Node::Ptr &node, std::string &name)
{
    if(node->is_node_category(AST::NodeType::Declaration)) {
        name = AST::cast_to<AST::Declaration>(node)->get_name();
        return true;
    }
    if(node->is_node_type(AST::NodeType::Function)) {
        name = AST::cast_to<AST::Function>(node)->get_name();
        return true;
    }
    if(node->is_node_type(AST::NodeType::Task)) {
        name = AST::cast_to<AST::Task>(node)->get_name();
        return true;
    }
    return false;
}

/// A reference carrying a `::` scope is either a value ref (Identifier and its
/// Call subtypes) or a type ref (NamedType). Both expose get_scope/get_name.
bool is_reference(const AST::Node::Ptr &node)
{
    return node->is_node_category(AST::NodeType::Identifier) ||
           node->is_node_type(AST::NodeType::NamedType);
}

AST::ScopeName::ListPtr ref_get_scope(const AST::Node::Ptr &node)
{
    if(node->is_node_category(AST::NodeType::Identifier)) {
        return AST::cast_to<AST::Identifier>(node)->get_scope();
    }
    return AST::cast_to<AST::NamedType>(node)->get_scope();
}

void ref_clear_scope(const AST::Node::Ptr &node)
{
    if(node->is_node_category(AST::NodeType::Identifier)) {
        AST::cast_to<AST::Identifier>(node)->set_scope(nullptr);
    } else {
        AST::cast_to<AST::NamedType>(node)->set_scope(nullptr);
    }
}

std::string ref_get_name(const AST::Node::Ptr &node)
{
    if(node->is_node_category(AST::NodeType::Identifier)) {
        return AST::cast_to<AST::Identifier>(node)->get_name();
    }
    return AST::cast_to<AST::NamedType>(node)->get_name();
}

/// Collect, in place, every node of `type` reachable from `root`.
void find_nodes(const AST::Node::Ptr &root, AST::NodeType type, std::list<AST::Node::Ptr> &out)
{
    if(!root) {
        return;
    }
    if(root->is_node_type(type)) {
        out.push_back(root);
    }
    AST::Node::ListPtr children = root->get_children();
    for(const AST::Node::Ptr &child : *children) {
        find_nodes(child, type, out);
    }
}

/// The compilation-unit definitions list (top-level scope), or null.
AST::Node::ListPtr top_level_definitions(const AST::Node::Ptr &root)
{
    if(root->is_node_type(AST::NodeType::Source)) {
        const auto &desc = AST::cast_to<AST::Source>(root)->get_description();
        return desc ? desc->get_definitions() : nullptr;
    }
    if(root->is_node_type(AST::NodeType::Description)) {
        return AST::cast_to<AST::Description>(root)->get_definitions();
    }
    return nullptr;
}

} // namespace

int PackageInliner::process(AST::Node::Ptr node, AST::Node::Ptr /*parent*/)
{
    if(!node) {
        return 0;
    }

    m_packages.clear();
    if(collect_packages(node) != 0) {
        return 1;
    }

    // Each module body is an importing scope; resolve them independently.
    std::list<AST::Node::Ptr> modules;
    find_nodes(node, AST::NodeType::Module, modules);
    for(const AST::Node::Ptr &m : modules) {
        if(process_scope(AST::cast_to<AST::Module>(m)->get_items(), m) != 0) {
            return 1;
        }
    }

    // The compilation unit itself is a scope (top-level imports / references).
    const AST::Node::ListPtr defs = top_level_definitions(node);
    if(defs) {
        if(process_scope(defs, node) != 0) {
            return 1;
        }
        // Strip the now-resolved package definitions from the output.
        for(auto it = defs->begin(); it != defs->end();) {
            it = (*it)->is_node_type(AST::NodeType::Package) ? defs->erase(it) : std::next(it);
        }
    }

    return 0;
}

int PackageInliner::collect_packages(const AST::Node::Ptr &root)
{
    std::list<AST::Node::Ptr> packages;
    find_nodes(root, AST::NodeType::Package, packages);

    for(const AST::Node::Ptr &p : packages) {
        const auto &pkg = AST::cast_to<AST::Package>(p);
        PackageEntry entry;
        entry.package = pkg;
        if(pkg->get_items()) {
            for(const AST::Node::Ptr &item : *pkg->get_items()) {
                std::string name;
                if(node_decl_name(item, name)) {
                    entry.symbols[name] = item;
                }
            }
        }
        if(m_packages.count(pkg->get_name())) {
            LOG_WARNING_N(pkg) << "package '" << pkg->get_name() << "' redefined; using the latest";
        }
        m_packages[pkg->get_name()] = entry;
    }
    return 0;
}

int PackageInliner::process_scope(const AST::Node::ListPtr &items, const AST::Node::Ptr &subtree)
{
    if(!items) {
        return 0;
    }

    ScopeTable table;
    ScopeCopies copies;
    copies.copies = std::make_shared<AST::Node::List>();

    // Local declarations shadow imports (§26.4–26.6).
    for(const AST::Node::Ptr &item : *items) {
        std::string name;
        if(node_decl_name(item, name)) {
            table.add_local(name, item);
            copies.names.insert(name);
        }
    }

    // Imports visible in this scope feed the table.
    for(const AST::Node::Ptr &item : *items) {
        if(!item->is_node_type(AST::NodeType::Import)) {
            continue;
        }
        const auto &imp = AST::cast_to<AST::Import>(item);
        const std::string &pkgname = imp->get_package();
        const std::string &sym = imp->get_symbol();

        auto pit = m_packages.find(pkgname);
        if(pit == m_packages.end()) {
            LOG_ERROR_N(imp) << "import of undefined package '" << pkgname << "'";
            return 1;
        }

        if(sym == "*") {
            for(const auto &kv : pit->second.symbols) {
                table.add_wildcard_import(kv.first, pkgname, kv.second);
            }
        } else {
            auto sit = pit->second.symbols.find(sym);
            if(sit == pit->second.symbols.end()) {
                LOG_ERROR_N(imp) << "package '" << pkgname << "' has no synthesizable symbol '"
                                 << sym << "'";
                return 1;
            }
            table.add_explicit_import(sym, pkgname, sit->second);
        }
    }

    if(rewrite_refs(subtree, subtree, table, copies) != 0) {
        return 1;
    }
    if(materialize_imports(items, table, copies) != 0) {
        return 1;
    }

    // Strip the imports (now resolved) and splice the copied declarations at the
    // front of the scope, in discovery order.
    for(auto it = items->begin(); it != items->end();) {
        it = (*it)->is_node_type(AST::NodeType::Import) ? items->erase(it) : std::next(it);
    }
    items->insert(items->begin(), copies.copies->begin(), copies.copies->end());

    return 0;
}

int PackageInliner::rewrite_refs(const AST::Node::Ptr &node, const AST::Node::Ptr &scope_root,
                                 const ScopeTable &table, ScopeCopies &copies)
{
    if(!node) {
        return 0;
    }
    // Do not descend into a nested scope: it is resolved on its own.
    if(node != scope_root && is_scope_boundary(node)) {
        return 0;
    }

    if(is_reference(node)) {
        const AST::ScopeName::ListPtr scope = ref_get_scope(node);
        const std::string name = ref_get_name(node);

        if(scope && !scope->empty()) {
            if(scope->size() == 1) {
                const std::string pkgname = scope->front()->get_name();
                auto pit = m_packages.find(pkgname);
                if(pit != m_packages.end()) {
                    auto sit = pit->second.symbols.find(name);
                    if(sit == pit->second.symbols.end()) {
                        LOG_ERROR_N(node) << "package '" << pkgname
                                          << "' has no synthesizable symbol '" << name << "'";
                        return 1;
                    }
                    if(ensure_copied(name, sit->second, copies) != 0) {
                        return 1;
                    }
                    ref_clear_scope(node); // now a local reference
                } else {
                    // Not a known package: $unit or a class scope — outside the
                    // PackageInliner remit (ADR-0004 §8). Leave untouched.
                    LOG_WARNING_N(node) << "scoped reference '" << pkgname << "::" << name
                                        << "' left unresolved (not a known package)";
                }
            } else {
                LOG_WARNING_N(node) << "multi-segment scoped reference to '" << name
                                    << "' left unresolved (out of PackageInliner scope)";
            }
        } else {
            // Unqualified: a multi-wildcard collision is an error only here, on use.
            bool ambiguous = false;
            table.lookup(name, &ambiguous);
            if(ambiguous) {
                LOG_ERROR_N(node) << "ambiguous reference to '" << name
                                  << "' (imported by several packages via wildcard)";
                return 1;
            }
        }
    }

    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        if(rewrite_refs(child, scope_root, table, copies) != 0) {
            return 1;
        }
    }
    return 0;
}

int PackageInliner::materialize_imports(const AST::Node::ListPtr &items, const ScopeTable &table,
                                        ScopeCopies &copies)
{
    for(const AST::Node::Ptr &item : *items) {
        if(!item->is_node_type(AST::NodeType::Import)) {
            continue;
        }
        const auto &imp = AST::cast_to<AST::Import>(item);
        auto pit = m_packages.find(imp->get_package());
        if(pit == m_packages.end()) {
            continue; // already diagnosed in process_scope
        }

        if(imp->get_symbol() == "*") {
            // Eager wildcard copy (decision 1): bring in every symbol, skipping a
            // local shadow or a name blocked by a multi-wildcard collision (the
            // latter errors only if actually referenced).
            for(const auto &kv : pit->second.symbols) {
                bool ambiguous = false;
                const ScopeTable::Binding *b = table.lookup(kv.first, &ambiguous);
                if(ambiguous || (b && b->origin == ScopeTable::Origin::Local)) {
                    continue;
                }
                if(ensure_copied(kv.first, kv.second, copies) != 0) {
                    return 1;
                }
            }
        } else {
            auto sit = pit->second.symbols.find(imp->get_symbol());
            if(sit != pit->second.symbols.end()) {
                if(ensure_copied(imp->get_symbol(), sit->second, copies) != 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int PackageInliner::ensure_copied(const std::string &name, const AST::Node::Ptr &decl,
                                  ScopeCopies &copies)
{
    if(copies.names.count(name)) {
        return 0; // a local, or already copied for this scope
    }
    copies.names.insert(name);
    copies.copies->push_back(decl->clone());
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
