// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_PACKAGE_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_PACKAGE_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>

#include <map>
#include <set>
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

private:
    /// One collected package: its node plus a name -> declaration index.
    struct PackageEntry
    {
        AST::Package::Ptr package;
        std::map<std::string, AST::Node::Ptr> symbols;
    };

    /// Per-scope accumulator while a single scope is being resolved.
    struct ScopeCopies
    {
        AST::Node::ListPtr copies;   ///< declarations to splice into the scope
        std::set<std::string> names; ///< names already present/copied (dedup)
    };

    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /// Index every top-level `package` into m_packages.
    int collect_packages(const AST::Node::Ptr &root);

    /// Resolve one importing scope: build its ScopeTable, rewrite references,
    /// materialize imported declarations, strip imports, splice copies in.
    int process_scope(const AST::Node::ListPtr &items, const AST::Node::Ptr &subtree);

    /// Recursively rewrite qualified/imported references inside one scope,
    /// stopping at nested scope boundaries (a separate scope is resolved on its
    /// own pass over the tree).
    int rewrite_refs(const AST::Node::Ptr &node, const AST::Node::Ptr &scope_root,
                     const ScopeTable &table, ScopeCopies &copies);

    /// Eagerly copy every explicitly/wildcard-imported declaration into the
    /// scope (decision 1), skipping local shadows and multi-wildcard conflicts.
    int materialize_imports(const AST::Node::ListPtr &items, const ScopeTable &table,
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
