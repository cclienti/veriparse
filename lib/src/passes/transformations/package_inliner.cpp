// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

#include <list>
#include <set>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// The compilation-unit scope name (`$unit::`), modelled as a pseudo-package.
const char *const UNIT_SCOPE = "$unit";

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

/// Collect the unqualified value/type reference names inside `node` (skipping
/// `::`-scoped refs, which name another package). Used to find a declaration's
/// same-package dependencies.
void collect_unqualified_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_category(AST::NodeType::Identifier)) {
        const auto scope = AST::cast_to<AST::Identifier>(node)->get_scope();
        if(!scope || scope->empty()) {
            names.insert(AST::cast_to<AST::Identifier>(node)->get_name());
        }
    } else if(node->is_node_type(AST::NodeType::NamedType)) {
        const auto scope = AST::cast_to<AST::NamedType>(node)->get_scope();
        if(!scope || scope->empty()) {
            names.insert(AST::cast_to<AST::NamedType>(node)->get_name());
        }
    }
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        collect_unqualified_names(child, names);
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
    // Single-source convenience (run()): collect then resolve this one source.
    if(!node) {
        return 0;
    }
    m_packages.clear();
    if(collect(node) != 0) {
        return 1;
    }
    return resolve(node);
}

int PackageInliner::run_units(const std::vector<AST::Node::Ptr> &sources)
{
    // Compilation order matters (§26.3): collect-then-resolve each unit before
    // the next, so a unit sees packages from earlier units but not later ones.
    m_packages.clear();
    for(const AST::Node::Ptr &source : sources) {
        if(collect(source) != 0) {
            return 1;
        }
        if(resolve(source) != 0) {
            return 1;
        }
    }
    return 0;
}

int PackageInliner::collect(const AST::Node::Ptr &source)
{
    // A package name is global across the design, so collection accumulates into
    // the shared registry across sources (no clear here).
    std::list<AST::Node::Ptr> packages;
    find_nodes(source, AST::NodeType::Package, packages);

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
        // Before its body is resolved, a package's contents are just its own
        // declarations; resolve_packages() re-indexes after pulling deps in.
        entry.contents = entry.symbols;
        if(m_packages.count(pkg->get_name())) {
            LOG_WARNING_N(pkg) << "package '" << pkg->get_name() << "' redefined; using the latest";
        }
        m_packages[pkg->get_name()] = entry;
    }
    return 0;
}

void PackageInliner::build_unit_scope(const AST::Node::Ptr &source)
{
    // The `$unit` (compilation-unit) pseudo-package holds the names this source's
    // top-level imports bring in, so `$unit::x` resolves through the same
    // single-segment path. (Top-level value declarations are not in the grammar,
    // so the unit scope holds only imported names.) It is per-source: overwrite
    // any previous source's `$unit`.
    PackageEntry unit; // entry.package stays null: there is no Package node
    const AST::Node::ListPtr defs = top_level_definitions(source);
    if(defs) {
        for(const AST::Node::Ptr &item : *defs) {
            if(!item->is_node_type(AST::NodeType::Import)) {
                continue;
            }
            const auto &imp = AST::cast_to<AST::Import>(item);
            auto pit = m_packages.find(imp->get_package());
            if(pit == m_packages.end()) {
                continue; // diagnosed when a scope consumes the import
            }
            if(imp->get_symbol() == "*") {
                for(const auto &kv : pit->second.symbols) {
                    unit.symbols[kv.first] = kv.second;
                }
            } else {
                auto sit = pit->second.symbols.find(imp->get_symbol());
                if(sit != pit->second.symbols.end()) {
                    unit.symbols[imp->get_symbol()] = sit->second;
                }
            }
        }
    }
    m_packages[UNIT_SCOPE] = unit;
}

int PackageInliner::resolve(const AST::Node::Ptr &source)
{
    if(!source) {
        return 0;
    }

    build_unit_scope(source);

    // Resolve package bodies first, in declaration order, so a package that
    // imports and uses another is self-contained before any module copies it.
    if(resolve_packages(source) != 0) {
        return 1;
    }

    // This source's own top-level imports are visible to its modules only (an
    // import in another file belongs to another compilation unit).
    std::list<AST::Node::Ptr> unit_imports;
    const AST::Node::ListPtr defs = top_level_definitions(source);
    if(defs) {
        for(const AST::Node::Ptr &item : *defs) {
            if(item->is_node_type(AST::NodeType::Import)) {
                unit_imports.push_back(item);
            }
        }
    }

    // Each module body is an importing scope; resolve them independently, with
    // this source's compilation-unit imports folded in.
    std::list<AST::Node::Ptr> modules;
    find_nodes(source, AST::NodeType::Module, modules);
    for(const AST::Node::Ptr &m : modules) {
        if(process_scope(AST::cast_to<AST::Module>(m)->get_items(), m, unit_imports) != 0) {
            return 1;
        }
    }

    // Strip the now-resolved compilation-unit imports and package definitions.
    if(defs) {
        for(auto it = defs->begin(); it != defs->end();) {
            const bool drop = (*it)->is_node_type(AST::NodeType::Import) ||
                              (*it)->is_node_type(AST::NodeType::Package);
            it = drop ? defs->erase(it) : std::next(it);
        }
    }

    return 0;
}

int PackageInliner::resolve_packages(const AST::Node::Ptr &source)
{
    // Packages appear in declaration order, which §26.3 guarantees is dependency
    // order (a package precedes the ones that import it). A package body is a
    // scope like a module body; resolve it with its OWN imports only (a
    // compilation-unit import does not eagerly flood every package). Then
    // re-index its contents to include any pulled-in dependencies.
    std::list<AST::Node::Ptr> packages;
    find_nodes(source, AST::NodeType::Package, packages);

    const std::list<AST::Node::Ptr> no_extra_imports;
    for(const AST::Node::Ptr &p : packages) {
        const auto &pkg = AST::cast_to<AST::Package>(p);
        if(process_scope(pkg->get_items(), p, no_extra_imports) != 0) {
            return 1;
        }
        // Re-index contents from the resolved body (own decls + pulled-in deps).
        // The interface (`symbols`) is left untouched — internal dependencies are
        // not exported (ADR-0004 §9.4).
        auto pit = m_packages.find(pkg->get_name());
        if(pit == m_packages.end() || !pkg->get_items()) {
            continue;
        }
        pit->second.contents.clear();
        for(const AST::Node::Ptr &item : *pkg->get_items()) {
            std::string name;
            if(node_decl_name(item, name)) {
                pit->second.contents[name] = item;
            }
        }

        // Fold re-exports into the interface, then strip the export nodes.
        if(fold_exports(p, pit->second) != 0) {
            return 1;
        }
        const AST::Node::ListPtr items = pkg->get_items();
        for(auto it = items->begin(); it != items->end();) {
            it = (*it)->is_node_type(AST::NodeType::Export) ? items->erase(it) : std::next(it);
        }
    }
    return 0;
}

int PackageInliner::fold_exports(const AST::Node::Ptr &package, PackageEntry &entry)
{
    // A re-export makes names the package imported visible to its importers, by
    // adding them to the interface (`symbols`). The names are taken from the
    // package's `contents` — what it actually imported (§26.6). Process in
    // declaration order so a re-export of an already-re-exporting package sees
    // its full interface (chained re-exports).
    const AST::Node::ListPtr items = AST::cast_to<AST::Package>(package)->get_items();
    if(!items) {
        return 0;
    }
    for(const AST::Node::Ptr &item : *items) {
        if(!item->is_node_type(AST::NodeType::Export)) {
            continue;
        }
        const auto &exp = AST::cast_to<AST::Export>(item);
        const std::string &pkgname = exp->get_package();
        const std::string &sym = exp->get_symbol();

        if(pkgname == "*") {
            // `export *::*;` — re-export everything the package imported.
            for(const auto &kv : entry.contents) {
                entry.symbols[kv.first] = kv.second;
            }
        } else if(sym == "*") {
            // `export pkg::*;` — re-export the names actually imported from pkg.
            auto qit = m_packages.find(pkgname);
            if(qit != m_packages.end()) {
                for(const auto &kv : qit->second.symbols) {
                    auto cit = entry.contents.find(kv.first);
                    if(cit != entry.contents.end()) {
                        entry.symbols[kv.first] = cit->second;
                    }
                }
            }
        } else {
            // `export pkg::name;` — error if it was not actually imported (§26.6).
            auto cit = entry.contents.find(sym);
            if(cit == entry.contents.end()) {
                LOG_ERROR_N(exp) << "export of '" << pkgname << "::" << sym
                                 << "' which the package did not import (§26.6)";
                return 1;
            }
            entry.symbols[sym] = cit->second;
        }
    }
    return 0;
}

int PackageInliner::process_scope(const AST::Node::ListPtr &items, const AST::Node::Ptr &subtree,
                                  const std::list<AST::Node::Ptr> &extra_imports)
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
            copies.bound[name] = item;
        }
    }

    // Imports visible in this scope: the scope's own, plus the compilation-unit
    // imports applied to every module.
    std::list<AST::Node::Ptr> imports;
    for(const AST::Node::Ptr &item : *items) {
        if(item->is_node_type(AST::NodeType::Import)) {
            imports.push_back(item);
        }
    }
    imports.insert(imports.end(), extra_imports.begin(), extra_imports.end());

    for(const AST::Node::Ptr &item : imports) {
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
    if(materialize_imports(imports, table, copies) != 0) {
        return 1;
    }

    // Strip the scope's own imports (the compilation-unit ones are stripped by
    // the caller) and splice the copied declarations at the front, in order.
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
                // Single segment: a package, or `$unit` (the compilation-unit
                // pseudo-package). Both resolve through m_packages.
                const std::string pkgname = scope->front()->get_name();
                auto pit = m_packages.find(pkgname);
                if(pit != m_packages.end()) {
                    auto sit = pit->second.symbols.find(name);
                    if(sit == pit->second.symbols.end()) {
                        LOG_ERROR_N(node)
                            << "'" << pkgname << "' has no synthesizable symbol '" << name << "'";
                        return 1;
                    }
                    // A qualified reference must bind to the package's declaration.
                    // If the name is already bound to a *different* declaration (a
                    // local, or another import), localizing it would silently
                    // rebind it to the wrong one — reject instead.
                    auto bit = copies.bound.find(name);
                    if(bit != copies.bound.end()) {
                        if(bit->second != sit->second) {
                            LOG_ERROR_N(node)
                                << "qualified reference '" << pkgname << "::" << name
                                << "' collides with a different declaration named '" << name
                                << "' already in scope (rename to disambiguate)";
                            return 1;
                        }
                    } else if(copy_symbol(pkgname, name, copies) != 0) {
                        return 1;
                    }
                    ref_clear_scope(node); // now a local reference
                } else {
                    // Not a known package nor `$unit`. In the synthesizable subset
                    // this is a missing package, or one whose unit is compiled
                    // later (§26.3) — a class scope (§8.18) is out of scope. Hard
                    // error rather than a silent unresolved reference.
                    LOG_ERROR_N(node) << "scoped reference '" << pkgname << "::" << name
                                      << "' to an unknown package (undeclared, or its unit is "
                                      << "compiled later — §26.3)";
                    return 1;
                }
            } else {
                // Multi-segment `A::B::T` is a class-scope chain (§8.18), not
                // nested packages — out of scope and not produced by the current
                // grammar. Defensive: report rather than mis-resolve.
                LOG_ERROR_N(node) << "multi-segment scoped reference to '" << name
                                  << "' is unsupported (class scope)";
                return 1;
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

int PackageInliner::materialize_imports(const std::list<AST::Node::Ptr> &imports,
                                        const ScopeTable &table, ScopeCopies &copies)
{
    // Gather the candidate names: every explicitly-imported name and every
    // interface symbol of every wildcard-imported package.
    std::set<std::string> candidates;
    for(const AST::Node::Ptr &item : imports) {
        const auto &imp = AST::cast_to<AST::Import>(item);
        auto pit = m_packages.find(imp->get_package());
        if(pit == m_packages.end()) {
            continue; // already diagnosed in process_scope
        }
        if(imp->get_symbol() == "*") {
            for(const auto &kv : pit->second.symbols) {
                candidates.insert(kv.first);
            }
        } else {
            candidates.insert(imp->get_symbol());
        }
    }

    // Eager copy: bring each candidate in, but copy the binding the ScopeTable
    // resolves it to, so the IEEE 1800-2017 §26.5 precedence holds — a local
    // shadows (already present, skip), an explicit import shadows a wildcard one,
    // and a name blocked by a multi-wildcard collision is skipped (it errors only
    // if actually referenced).
    for(const std::string &name : candidates) {
        bool ambiguous = false;
        const ScopeTable::Binding *b = table.lookup(name, &ambiguous);
        if(ambiguous || !b || b->origin == ScopeTable::Origin::Local) {
            continue;
        }
        if(copy_symbol(b->package, name, copies) != 0) {
            return 1;
        }
    }
    return 0;
}

int PackageInliner::copy_symbol(const std::string &pkgname, const std::string &name,
                                ScopeCopies &copies)
{
    if(copies.bound.count(name)) {
        return 0; // a local shadow, or already copied for this scope
    }
    auto pit = m_packages.find(pkgname);
    if(pit == m_packages.end()) {
        return 0; // package validated by the caller; defensive
    }
    auto cit = pit->second.contents.find(name);
    if(cit == pit->second.contents.end()) {
        return 0; // symbol validated by the caller; defensive
    }

    // Clone the declaration and bind the name first (dedup / cycle guard), then
    // copy the same-package symbols it transitively depends on (ADR-0004 §9.3),
    // and splice the dependencies BEFORE this declaration so the output reads
    // declared-before-use.
    const AST::Node::Ptr clone = cit->second->clone();
    copies.bound[name] = cit->second;

    std::set<std::string> refs;
    collect_unqualified_names(clone, refs);
    for(const std::string &ref : refs) {
        if(ref != name && pit->second.contents.count(ref) && !copies.bound.count(ref)) {
            if(copy_symbol(pkgname, ref, copies) != 0) {
                return 1;
            }
        }
    }
    copies.copies->push_back(clone);
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
