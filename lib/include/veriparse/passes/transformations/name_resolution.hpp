// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_NAME_RESOLUTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_NAME_RESOLUTION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/scope_table.hpp>

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
 * @brief Broad name-resolution pass (ADR-0006): one symbol-aware sweep that
 * asks, for each node the parser left deliberately unresolved (ADR-0003), what
 * its name declares — and re-tags the node to its precise form.
 *
 * The pass never guesses: a name it cannot classify leaves the neutral node in
 * place with a diagnostic. It is designed to run after PackageInliner (every
 * scope self-contained, `::` scopes gone); run standalone, imported names
 * simply stay unresolved.
 *
 * Resolution is driven by two tables:
 *  - a design index (module/interface definitions across all sources) — the
 *    fallback namespace of design elements;
 *  - a lexical scope stack of ScopeTable, consulted first and innermost-out
 *    (a local binding shadows a design-level name).
 */
class NameResolution : public TransformationBase
{
public:
    /**
     * @brief Resolve a whole design: index every source's definitions, then
     * resolve each source against the complete index.
     * @return zero on success.
     */
    int run_design(const std::vector<AST::Node::Ptr> &sources);

protected:
    /**
     * @brief Single-source entry: index the source's definitions and resolve
     * it (run_design covers the multi-source case).
     */
    virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
    struct DesignEntry
    {
        ScopeTable::SymbolKind kind;
        AST::Node::Ptr node;
    };

    /// Index a source's design-level definitions (Module / Interface). A
    /// duplicate definition name is a hard error (§3.13 definitions name space).
    int index_design(const AST::Node::Ptr &source);

    /// Resolve one source against the design index.
    int resolve_source(const AST::Node::Ptr &source);

    /// Recursive resolution walk; pushes/pops lexical scopes.
    int walk(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

    /// Add one scope item's binding(s) to the table (declarations, subroutine
    /// names, genvars, and the enum items of an inline EnumType).
    void add_scope_item(ScopeTable &table, const AST::Node::Ptr &item);
    void add_scope_items(ScopeTable &table, const AST::Node::ListPtr &items);

    /// Innermost-out lookup through the lexical scope stack.
    const ScopeTable::Binding *lookup(const std::string &name) const;

    /// Re-tag a neutral statement Call from the declared kind of its callee
    /// (ADR-0006 §4.1). Never fails; unresolvable calls stay neutral.
    void retag_call(const AST::Call::Ptr &call, const AST::Node::Ptr &parent);

    /// Design-index entry for a name, or nullptr. The lexical stack is
    /// consulted first (ADR-0006 §4.2): a local binding shadows a design-level
    /// name, so a shadowed name never reaches the index.
    const DesignEntry *design_lookup_unshadowed(const std::string &name) const;

    /// Re-tag every Instance of a list whose target names an interface
    /// (ADR-0002 §2.4). Instantiation targets are design-element references,
    /// never lexically shadowed.
    void retag_instances(const AST::Instancelist::Ptr &instancelist);

    /// Resolve a deferred module port (ADR-0006 §4.2): promote the bare
    /// Arg{NamedType} form to an interface or typed-net port, and undo the
    /// inherited direction of a port that proves to be an interface.
    int resolve_port(const AST::Port::Ptr &port);

    /// §25.9: an interface used as a data type outside a port requires
    /// `virtual` — hard error.
    int check_interface_as_data_type(const AST::Declaration::Ptr &decl);

    /// ADR-0006 §4.4: `N'(x)` parsed as TypeCast{NamedType{N}} becomes a
    /// SizeCast when N binds a value; stays a TypeCast when N is a type.
    void retag_typecast(const AST::TypeCast::Ptr &cast, const AST::Node::Ptr &parent);

    /// ADR-0006 §4.5: `type(N)` parsed as TypeOpExpr becomes TypeOpType when
    /// the bare identifier N binds a type.
    void retag_typeop(const AST::TypeOpExpr::Ptr &typeop, const AST::Node::Ptr &parent);

    /// RAII-less scope helpers around walk() recursion.
    int walk_in_scope(const ScopeTable &table, const AST::Node::Ptr &node);

    std::map<std::string, DesignEntry> m_design;
    std::vector<ScopeTable> m_scopes;
    int m_interface_depth = 0;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
