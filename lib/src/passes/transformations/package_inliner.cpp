// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

#include <list>
#include <map>
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

/// A module/interface/package opens a separate name scope; a scope is resolved
/// on its own traversal, so reference rewriting stops at these boundaries.
bool is_scope_boundary(const AST::Node::Ptr &node)
{
    return node->is_node_type(AST::NodeType::Module) ||
           node->is_node_type(AST::NodeType::Interface) ||
           node->is_node_type(AST::NodeType::Package);
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

void collect_bound_names_rec(const AST::Node::Ptr &node,
                             const std::function<void(const std::string &)> &visit)
{
    if(!node) {
        return;
    }
    ScopeTable::for_each_bound_name(node, visit);
    AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        collect_bound_names_rec(child, visit);
    }
}

/// Collect the names DECLARED inside `node` — its own name plus subroutine args,
/// local variables, genvars, inline-enum enumerators (every bound name). A reference to
/// such a name is bound within the declaration, so it is NOT a same-package
/// dependency (a function arg may legitimately share a package symbol's name).
void collect_bound_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    const std::function<void(const std::string &)> visit = [&names](const std::string &name) {
        names.insert(name);
    };
    collect_bound_names_rec(node, visit);
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
        bool duplicate = false;
        if(pkg->get_items()) {
            for(const AST::Node::Ptr &item : *pkg->get_items()) {
                ScopeTable::for_each_bound_name(
                    item, [&entry, &pkg, &item, &duplicate](const std::string &name) {
                        // Two package items binding one name is a duplicate
                        // declaration in the package scope (§26.2) — diagnose,
                        // never bind last-wins.
                        const auto ins = entry.symbols.emplace(name, item);
                        if(!ins.second && ins.first->second != item) {
                            LOG_ERROR_N(item) << "duplicate declaration of '" << name
                                              << "' in package '" << pkg->get_name() << "'";
                            duplicate = true;
                            return;
                        }
                        entry.origin[name] = pkg->get_name(); // an own decl originates here
                    });
            }
        }
        if(duplicate) {
            return 1;
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
    // top-level imports bring in — so `$unit::x` resolves through the same
    // single-segment path — plus its own top-level typedefs (§26.3; the only
    // unit-scope declaration the grammar admits, ADR-0009 §6). It is per-source:
    // overwrite any previous source's `$unit`.
    PackageEntry unit; // entry.package stays null: there is no Package node
    const AST::Node::ListPtr defs = top_level_definitions(source);
    if(defs) {
        for(const AST::Node::Ptr &item : *defs) {
            if(item->is_node_type(AST::NodeType::Typedef)) {
                const auto &tdef = AST::cast_to<AST::Typedef>(item);
                unit.symbols[tdef->get_name()] = item;
                unit.origin[tdef->get_name()] = UNIT_SCOPE;
                continue;
            }
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
    // `$unit` is never resolved as a body, so mirror its interface into contents
    // (the source `copy_symbol` reads when a `$unit::x` reference pulls it in).
    unit.contents = unit.symbols;
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
    // import in another file belongs to another compilation unit). Capture the
    // Import nodes first: resolving the unit scope below strips them from the
    // tree, but every module/interface scope still consumes them.
    std::list<AST::Node::Ptr> unit_imports;
    bool has_unit_typedef = false;
    const AST::Node::ListPtr defs = top_level_definitions(source);
    if(defs) {
        for(const AST::Node::Ptr &item : *defs) {
            if(item->is_node_type(AST::NodeType::Import)) {
                unit_imports.push_back(item);
            } else if(item->is_node_type(AST::NodeType::Typedef)) {
                has_unit_typedef = true;
            }
        }
    }

    // A top-level typedef may itself reference package symbols — qualified
    // (`typedef p::T u_t;`) or through the unit's imports. The unit scope is an
    // importing scope of its own (§26.3): resolve it before any consuming scope
    // copies its typedefs, so every clone spliced downstream is scope-free.
    // Resolution may splice package dependency copies at the unit scope's head;
    // register them as unit symbols so consuming scopes pull them in too.
    if(has_unit_typedef) {
        const std::list<AST::Node::Ptr> no_extra_imports;
        if(process_scope(defs, source, no_extra_imports) != 0) {
            return 1;
        }
        PackageEntry &unit = m_packages[UNIT_SCOPE];
        for(const AST::Node::Ptr &item : *defs) {
            ScopeTable::for_each_bound_name(item, [&unit, &item](const std::string &name) {
                unit.symbols[name] = item;
                unit.contents[name] = item;
                unit.origin.emplace(name, UNIT_SCOPE);
            });
        }
    }

    // A top-level typedef is directly visible in the unit's modules (§26.3): it
    // rides the same path as a synthesized explicit `import $unit::T`, so
    // shadowing, dedup and collision handling are inherited unchanged.
    if(defs) {
        for(const AST::Node::Ptr &item : *defs) {
            if(item->is_node_type(AST::NodeType::Typedef)) {
                const auto &imp =
                    std::make_shared<AST::Import>(item->get_filename(), item->get_line());
                imp->set_package(UNIT_SCOPE);
                imp->set_symbol(AST::cast_to<AST::Typedef>(item)->get_name());
                unit_imports.push_back(imp);
            }
        }
    }

    // Each module or interface body is an importing scope; resolve them
    // independently, with this source's compilation-unit imports folded in.
    std::list<AST::Node::Ptr> modules;
    find_nodes(source, AST::NodeType::Module, modules);
    for(const AST::Node::Ptr &m : modules) {
        if(process_scope(AST::cast_to<AST::Module>(m)->get_items(), m, unit_imports) != 0) {
            return 1;
        }
    }
    std::list<AST::Node::Ptr> interfaces;
    find_nodes(source, AST::NodeType::Interface, interfaces);
    for(const AST::Node::Ptr &i : interfaces) {
        if(process_scope(AST::cast_to<AST::Interface>(i)->get_items(), i, unit_imports) != 0) {
            return 1;
        }
    }

    // Strip everything the scopes have consumed: the compilation-unit imports,
    // typedefs (copied into their consuming scopes), package definitions, and
    // any dependency declaration the unit-scope resolution spliced (a package
    // parameter or subroutine pulled in by a unit typedef). Only design
    // elements remain — the grammar admits nothing else at this level.
    if(defs) {
        for(auto it = defs->begin(); it != defs->end();) {
            const bool keep = (*it)->is_node_type(AST::NodeType::Module) ||
                              (*it)->is_node_type(AST::NodeType::Interface) ||
                              (*it)->is_node_type(AST::NodeType::Pragmalist);
            it = keep ? std::next(it) : defs->erase(it);
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

        // Capture this package's imports before process_scope strips them: the
        // wildcard ones (an explicit `export pkg::name` may force-import from one)
        // and the explicit ones (name -> source package, for origin recovery).
        std::set<std::string> wildcard_pkgs;
        std::map<std::string, std::string> explicit_imports;
        if(pkg->get_items()) {
            for(const AST::Node::Ptr &item : *pkg->get_items()) {
                if(!item->is_node_type(AST::NodeType::Import)) {
                    continue;
                }
                const auto &imp = AST::cast_to<AST::Import>(item);
                if(imp->get_symbol() == "*") {
                    wildcard_pkgs.insert(imp->get_package());
                } else {
                    explicit_imports[imp->get_symbol()] = imp->get_package();
                }
            }
        }

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
            ScopeTable::for_each_bound_name(item, [&pit, &item](const std::string &name) {
                pit->second.contents[name] = item;
            });
        }

        // Record the defining package of each IMPORTED content name (own decls
        // already have it from collect()), so a re-export — and a downstream
        // multi-path import — sees the original origin (ADR-0004 §9.5). An
        // explicitly imported declaration binds its sibling names too (an
        // imported enum typedef brings its enumerators), so attribute every
        // name the imported item binds, not only the imported one.
        for(const auto &imp : explicit_imports) {
            auto iit = pit->second.contents.find(imp.first);
            if(iit == pit->second.contents.end()) {
                continue;
            }
            const std::string &src_pkg = imp.second;
            ScopeTable::for_each_bound_name(
                iit->second, [this, &pit, &src_pkg](const std::string &name) {
                    if(!pit->second.origin.count(name)) {
                        pit->second.origin[name] = defining_package_of(src_pkg, name);
                    }
                });
        }
        for(const auto &kv : pit->second.contents) {
            if(pit->second.origin.count(kv.first)) {
                continue;
            }
            for(const std::string &q : wildcard_pkgs) {
                auto qit = m_packages.find(q);
                if(qit != m_packages.end() && qit->second.symbols.count(kv.first)) {
                    pit->second.origin[kv.first] = defining_package_of(q, kv.first);
                    break;
                }
            }
        }

        // Fold re-exports into the interface, then strip the export nodes.
        if(fold_exports(p, pit->second, wildcard_pkgs) != 0) {
            return 1;
        }
        const AST::Node::ListPtr items = pkg->get_items();
        for(auto it = items->begin(); it != items->end();) {
            it = (*it)->is_node_type(AST::NodeType::Export) ? items->erase(it) : std::next(it);
        }
    }
    return 0;
}

std::string PackageInliner::defining_package_of(const std::string &pkg,
                                                const std::string &name) const
{
    auto pit = m_packages.find(pkg);
    if(pit != m_packages.end()) {
        auto oit = pit->second.origin.find(name);
        if(oit != pit->second.origin.end()) {
            return oit->second;
        }
    }
    return pkg; // unrecorded: the symbol is pkg's own declaration
}

int PackageInliner::fold_exports(const AST::Node::Ptr &package, PackageEntry &entry,
                                 const std::set<std::string> &wildcard_pkgs)
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

        // The defining package of each content name is already recorded (own
        // decls in collect(), imported names in resolve_packages()), so these
        // branches only expose names to the interface; the force-import case below
        // adds a new symbol and records its origin itself.
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
            // `export pkg::name;`. If already imported, just expose it. Otherwise
            // the explicit export *forces* the import from pkg (§26.6: "the export
            // shall be considered to be a reference and shall import the
            // declaration"); error only if pkg has no such symbol.
            auto cit = entry.contents.find(sym);
            if(cit != entry.contents.end()) {
                entry.symbols[sym] = cit->second;
            } else {
                // Not yet imported: legal only if pkg is wildcard-imported (so sym
                // is a candidate) and pkg actually has it — then the export forces
                // the import. Otherwise it is an error (§26.6).
                auto qit = m_packages.find(pkgname);
                if(!wildcard_pkgs.count(pkgname) || qit == m_packages.end() ||
                   !qit->second.contents.count(sym)) {
                    LOG_ERROR_N(exp) << "export of '" << pkgname << "::" << sym
                                     << "' which is not a candidate for import (§26.6)";
                    return 1;
                }
                // Force-import sym (and its same-package deps) into the package.
                ScopeCopies forced;
                forced.copies = std::make_shared<AST::Node::List>();
                forced.bound = entry.contents; // dedup against what is already in
                if(copy_symbol(pkgname, sym, forced) != 0) {
                    return 1;
                }
                items->insert(items->begin(), forced.copies->begin(), forced.copies->end());
                for(const AST::Node::Ptr &copied : *forced.copies) {
                    ScopeTable::for_each_bound_name(copied,
                                                    [&entry, &copied](const std::string &name) {
                                                        entry.contents[name] = copied;
                                                    });
                }
                entry.symbols[sym] = entry.contents[sym];
                entry.origin[sym] = defining_package_of(pkgname, sym);
            }
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

    // Local declarations shadow imports (§26.4–26.6). The table binds the
    // DENOTED node (the EnumItem for an enumerator — its kind is VALUE, not
    // its typedef's TYPE); the dedup map keeps the item, the unit of copy.
    for(const AST::Node::Ptr &item : *items) {
        ScopeTable::for_each_bound_name(
            item, [&table, &copies, &item](const std::string &name, const AST::Node::Ptr &denoted) {
                table.add_local(name, denoted);
                copies.bound[name] = item;
            });
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
                table.add_wildcard_import(kv.first, pkgname, kv.second,
                                          defining_package_of(pkgname, kv.first));
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
            // Unqualified: resolve against the scope's imports. A multi-wildcard
            // collision is an error only here, on use (§26.5).
            bool ambiguous = false;
            const ScopeTable::Binding *b = table.lookup(name, &ambiguous);
            if(ambiguous) {
                LOG_ERROR_N(node) << "ambiguous reference to '" << name
                                  << "' (imported by several packages via wildcard)";
                return 1;
            }
            // A wildcard import is lazy: a name is "actually imported" only when
            // referenced (§26.5/§26.6), so copy it here, on use. (Local and
            // explicit-import names are already present in the scope.)
            if(b && b->origin == ScopeTable::Origin::WildcardImport) {
                if(copy_symbol(b->package, name, copies) != 0) {
                    return 1;
                }
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
    // Only explicit imports (`import pkg::x;`) are materialized here: an explicit
    // import is "actually imported" regardless of use (§26.6). Wildcard imports
    // are lazy — a wildcard name is copied on use, in rewrite_refs (§26.5).
    for(const AST::Node::Ptr &item : imports) {
        const auto &imp = AST::cast_to<AST::Import>(item);
        if(imp->get_symbol() == "*") {
            continue; // wildcard: lazy
        }
        // Copy the binding the ScopeTable resolves the name to, so a local
        // declaration of the same name still shadows the explicit import.
        bool ambiguous = false;
        const ScopeTable::Binding *b = table.lookup(imp->get_symbol(), &ambiguous);
        if(ambiguous || !b || b->origin == ScopeTable::Origin::Local) {
            continue;
        }
        if(copy_symbol(b->package, imp->get_symbol(), copies) != 0) {
            return 1;
        }
    }
    return 0;
}

int PackageInliner::copy_symbol(const std::string &pkgname, const std::string &name,
                                ScopeCopies &copies)
{
    auto pit = m_packages.find(pkgname);
    if(pit == m_packages.end()) {
        return 0; // package validated by the caller; defensive
    }
    auto cit = pit->second.contents.find(name);
    if(cit == pit->second.contents.end()) {
        return 0; // symbol validated by the caller; defensive
    }
    auto nit = copies.bound.find(name);
    if(nit != copies.bound.end()) {
        // Already bound: a local shadow (the caller resolved precedence), or
        // already copied for this scope. But bound to a DIFFERENT declaration
        // than the one requested (e.g. a sibling name of an earlier copy),
        // skipping would silently rebind every reference — reject.
        if(nit->second != cit->second && !nit->second->is_equal(*cit->second, false)) {
            LOG_ERROR_N(cit->second)
                << "importing '" << pkgname << "::" << name << "' collides with a different "
                << "declaration named '" << name << "' already in scope (rename to disambiguate)";
            return 1;
        }
        return 0;
    }

    // Clone the declaration and bind EVERY name it declares first (dedup /
    // cycle guard — a later reference to the typedef behind an enumerator, or
    // to a sibling enumerator, must not clone the declaration again), then
    // copy the same-package symbols it transitively depends on (ADR-0004
    // §9.3), and splice the dependencies BEFORE this declaration so the
    // output reads declared-before-use.
    //
    // A sibling bound name already bound to a DIFFERENT declaration (a local,
    // or another import's copy) cannot be registered: splicing would emit a
    // duplicate declaration, and skipping would silently rebind references to
    // the wrong declaration — reject, as for a qualified-reference collision.
    // A binding that is structurally EQUAL is the same declaration reached
    // through another path (e.g. a clone of it) and stays as-is.
    const AST::Node::Ptr clone = cit->second->clone();
    std::string collision;
    ScopeTable::for_each_bound_name(
        cit->second, [&copies, &cit, &collision](const std::string &bound_name) {
            auto bit = copies.bound.find(bound_name);
            if(bit != copies.bound.end() && bit->second != cit->second &&
               !bit->second->is_equal(*cit->second, false)) {
                if(collision.empty()) {
                    collision = bound_name;
                }
                return;
            }
            copies.bound[bound_name] = cit->second;
        });
    if(!collision.empty()) {
        LOG_ERROR_N(cit->second) << "importing '" << pkgname << "::" << name << "' also binds '"
                                 << collision << "', which collides with a different declaration "
                                 << "named '" << collision
                                 << "' already in scope (rename to disambiguate)";
        return 1;
    }

    // Dependencies are the FREE same-package names: referenced, but not declared
    // within the clone (a subroutine arg/local shadows a same-named package symbol
    // and must not pull it in).
    std::set<std::string> refs;
    std::set<std::string> bound;
    collect_unqualified_names(clone, refs);
    collect_bound_names(clone, bound);
    for(const std::string &ref : refs) {
        if(ref == name || bound.count(ref)) {
            continue;
        }
        auto dit = pit->second.contents.find(ref);
        if(dit == pit->second.contents.end()) {
            continue; // not a same-package symbol
        }
        auto bit = copies.bound.find(ref);
        if(bit != copies.bound.end()) {
            // Already in scope. Fine when it IS this dependency (copied
            // earlier, or an equal clone of it); a DIFFERENT declaration
            // would silently rebind the copied reference — reject.
            if(bit->second != dit->second && !bit->second->is_equal(*dit->second, false)) {
                LOG_ERROR_N(clone) << "'" << pkgname << "::" << name << "' depends on '" << pkgname
                                   << "::" << ref << "', which is shadowed by a different "
                                   << "declaration named '" << ref
                                   << "' in the importing scope (rename to disambiguate)";
                return 1;
            }
            continue;
        }
        if(copy_symbol(pkgname, ref, copies) != 0) {
            return 1;
        }
    }
    copies.copies->push_back(clone);
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
