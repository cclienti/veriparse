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
 *   - wildcard imports are eager (decision 1) — unused copies are removed by the
 *     existing DeadcodeElimination pass.
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

private:
    /// One collected package: its node plus a name -> declaration index.
    struct PackageEntry
    {
        AST::Package::Ptr package;
        std::map<std::string, AST::Node::Ptr> symbols;
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
    /// scope (decision 1), skipping local shadows and multi-wildcard conflicts.
    int materialize_imports(const std::list<AST::Node::Ptr> &imports, const ScopeTable &table,
                            ScopeCopies &copies);

    /// Clone `decl` into the scope's copy list unless its name is already
    /// present (a local or an earlier copy). Returns 0 on success.
    int ensure_copied(const std::string &name, const AST::Node::Ptr &decl, ScopeCopies &copies);

    std::map<std::string, PackageEntry> m_packages;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
