// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_PACKAGE_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_PACKAGE_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/**
 * @brief Resolve SystemVerilog `package` / `import` into self-contained output
 * (ADR-0004, IEEE 1800-2017 §26).
 *
 * `import` is name lookup, not text substitution: the pass collects every
 * `package … endpackage`, then for each importing scope (a module body) it
 * copies the referenced synthesizable declarations in and rewrites the
 * references to be local, finally stripping all `package`/`import` nodes.
 *
 * Collect-then-resolve is sound because §26.3 / §6.18 forbid forward references
 * to a package or a type, so one collection sweep suffices.
 *
 * Scope, per ADR-0004:
 *   - resolves explicit (`import pkg::x;`) and wildcard (`import pkg::*;`)
 *     imports, and qualified references (`pkg::X`);
 *   - covers the synthesizable subset (`parameter`, `localparam`, `typedef`,
 *     `enum`, constant `function`, `task`);
 *   - wildcard imports are eager — every candidate name is copied in, and the
 *     existing DeadcodeElimination pass removes the unused ones.
 *
 * Compilation units. A `package` name is global across the whole design (§26.2),
 * but `import` directives, `$unit` declarations and macros are scoped to a single
 * compilation unit (one source file). So collection is global and resolution is
 * per-unit:
 *   - `collect(source)` indexes that source's packages into the shared registry;
 *     call it for EVERY source first, so a module in one file can reference a
 *     package defined in another (B refers to X across files);
 *   - `resolve(source)` then resolves that source's modules against the shared
 *     packages plus *that source's own* top-level imports / `$unit` — a top-level
 *     import in another file is not visible here.
 *   - `run(source)` is the single-source convenience: collect then resolve it.
 *
 * A qualified reference that would collide with a *different* declaration already
 * in scope is a hard error (rather than a silent rebind). Multi-segment `::`
 * (`A::B::T`) is a class-scope chain (§8.18), out of scope and not yet parsed.
 *
 * It does NOT concretize a `NamedType` into a built-in `DataType` (that is the
 * sibling typedef-resolution concern, ADR-0004 §7); it only makes the named
 * declaration local so a later pass can resolve it.
 *
 * Run after parsing, before ParameterInliner / ModuleFlattener.
 */
class PackageInliner : public TransformationBase
{
public:
    PackageInliner() = default;

    /// Index every `package` in `source` into the shared registry (accumulates;
    /// does not clear). Call once per source before resolving any of them.
    int collect(const AST::Node::Ptr &source);

    /// Resolve one compilation unit: its modules' imports and qualified
    /// references resolve against the shared packages plus this source's own
    /// top-level imports / `$unit`; package and import nodes are stripped.
    int resolve(const AST::Node::Ptr &source);

    /// Resolve a list of compilation units **in order**: each is collected then
    /// resolved before the next (clears the registry first). So a unit may
    /// reference a package from an *earlier* unit but not a *later* one
    /// (§26.3 declared-before-use). `sources` must be in compilation order — for
    /// veriflat, the command-line file order. Returns 0 on success.
    int run_units(const std::vector<AST::Node::Ptr> &sources);

private:
    /// One collected package (ADR-0004 §9.4). `symbols` is the package's
    /// *interface* — the names it exposes to `import`/`pkg::x` (its own
    /// declarations, plus re-exports once those land). `contents` is everything
    /// physically in the package after its body is resolved, including the
    /// same-package dependencies pulled in for its own internal use — used to
    /// copy a symbol together with what it transitively depends on.
    struct PackageEntry
    {
        AST::Package::Ptr package;
        std::map<std::string, AST::Node::Ptr> symbols;  ///< interface (import-visible)
        std::map<std::string, AST::Node::Ptr> contents; ///< all items, for dependency copy
    };

    /// Per-scope accumulator while a single scope is being resolved. `bound` maps
    /// each name present in the scope (a local, or an already-copied import) to
    /// its declaration, so dedup and qualified-reference collisions are detected.
    struct ScopeCopies
    {
        AST::Node::ListPtr copies;                   ///< decls to splice into the scope
        std::map<std::string, AST::Node::Ptr> bound; ///< name -> declaration in scope
    };

    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /// Build the `$unit` pseudo-package for one source from its top-level imports
    /// (overwrites any previous source's `$unit`; resolution is sequential).
    void build_unit_scope(const AST::Node::Ptr &source);

    /// Resolve every package body in `source` as a scope (ADR-0004 §9.2), in
    /// declaration order, then re-index its `contents`. Makes a package that
    /// imports and uses another self-contained before any module copies from it.
    int resolve_packages(const AST::Node::Ptr &source);

    /// Fold a package's re-exports (`export pkg::*;`, …) into its interface
    /// `symbols`, from what it actually imported into `contents` (IEEE 1800-2017
    /// §26.6). Errors on an explicit export of a name that was not imported.
    int fold_exports(const AST::Node::Ptr &package, PackageEntry &entry);

    /// Resolve one importing scope: build its ScopeTable from the scope's own
    /// imports plus `extra_imports` (the compilation-unit imports), rewrite
    /// references, materialize imported declarations, strip imports, splice in.
    int process_scope(const AST::Node::ListPtr &items, const AST::Node::Ptr &subtree,
                      const std::list<AST::Node::Ptr> &extra_imports);

    /// Recursively rewrite qualified/imported references inside one scope,
    /// stopping at nested scope boundaries (a separate scope is resolved on its
    /// own pass over the tree).
    int rewrite_refs(const AST::Node::Ptr &node, const AST::Node::Ptr &scope_root,
                     const ScopeTable &table, ScopeCopies &copies);

    /// Eagerly copy every explicitly/wildcard-imported declaration into the
    /// scope, copying the binding name resolution selects so that an explicit
    /// import shadows a wildcard one; local shadows and multi-wildcard conflicts
    /// are skipped.
    int materialize_imports(const std::list<AST::Node::Ptr> &imports, const ScopeTable &table,
                            ScopeCopies &copies);

    /// Copy symbol `name` from package `pkgname` into the scope, together with the
    /// same-package symbols it transitively depends on (ADR-0004 §9.3). Looks the
    /// declaration and its dependencies up in the package's `contents`.
    int copy_symbol(const std::string &pkgname, const std::string &name, ScopeCopies &copies);

    std::map<std::string, PackageEntry> m_packages;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
