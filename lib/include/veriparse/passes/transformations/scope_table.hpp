// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_SCOPE_TABLE
#define VERIPARSE_PASSES_TRANSFORMATIONS_SCOPE_TABLE

#include <veriparse/AST/nodes.hpp>

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
 * @brief Name bindings for a single scope, in IEEE 1800-2017 §26.4–26.6
 * precedence order.
 *
 * A scope (a module body, the compilation unit, …) binds unqualified names to
 * declarations from three sources, in decreasing priority:
 *   1. a local declaration,
 *   2. an explicit import (`import pkg::x;`),
 *   3. a wildcard import (`import pkg::*;`).
 *
 * A name offered by two or more wildcards is NOT an error in itself (§26.6); it
 * becomes a hard error only when that name is actually referenced — hence
 * lookup() reports the ambiguity on use rather than at build time.
 *
 * This is the reusable seam of ADR-0004: PackageInliner is its first client;
 * the broad name-resolution pass (ADR-0006) asks a binding's *kind* instead of
 * copying its declaration.
 */
class ScopeTable
{
public:
    enum class Origin
    {
        Local,
        ExplicitImport,
        WildcardImport
    };

    /**
     * @brief What a bound name declares (ADR-0006 §2). Classification is
     * syntactic on the declaration node — the node type fully determines the
     * kind; no inference, no type-checking.
     */
    enum class SymbolKind
    {
        UNKNOWN,   ///< not a symbol-declaring node — callers treat as unbound
        VALUE,     ///< Var, Net, Param (value), Arg, Member, Genvar, EnumItem
        TYPE,      ///< Typedef, TypeParam — a name that denotes a data type
        FUNCTION,  ///< Function declaration
        TASK,      ///< Task declaration
        MODULE,    ///< design-level Module definition
        INTERFACE, ///< design-level Interface definition
    };

    struct Binding
    {
        AST::Node::Ptr decl;          ///< the bound declaration (Param/Var/Typedef/…)
        std::string package;          ///< source package ("" for a local binding)
        Origin origin;                ///< how the name became visible
        std::string defining_package; ///< package the declaration ORIGINATES in — the
                                      ///< same for a symbol reached via a re-export, so
                                      ///< two same-origin wildcard paths do not conflict
                                      ///< (IEEE 1800-2017 §26.6)
        SymbolKind kind;              ///< what the declaration declares (from classify())
    };

    /**
     * @brief The SymbolKind a declaration node declares, from its node type
     * alone. UNKNOWN for a null or non-declaring node.
     */
    static SymbolKind classify(const AST::Node::Ptr &decl);

    /**
     * @brief Register a name declared in the scope itself. A local shadows any
     * import of the same name.
     */
    void add_local(const std::string &name, AST::Node::Ptr decl);

    /**
     * @brief Register an explicitly imported name (`import pkg::name;`). An
     * explicit import shadows a wildcard of the same name.
     */
    void add_explicit_import(const std::string &name, const std::string &package,
                             AST::Node::Ptr decl);

    /**
     * @brief Register a wildcard-imported name (`import pkg::*;`). Several
     * wildcards may offer the same name; the conflict surfaces only in lookup(),
     * and only when their `defining_package` differs (§26.6).
     */
    void add_wildcard_import(const std::string &name, const std::string &package,
                             AST::Node::Ptr decl, const std::string &defining_package);

    /**
     * @brief Resolve an unqualified name to its binding, applying precedence.
     *
     * @param name       the unqualified name to resolve.
     * @param ambiguous  set to true when the name is blocked by a multi-wildcard
     *                   collision (the §26.6 on-use error); never null is required.
     * @return the winning binding, or nullptr when unbound or ambiguous.
     */
    const Binding *lookup(const std::string &name, bool *ambiguous = nullptr) const;

private:
    std::map<std::string, Binding> m_local;
    std::map<std::string, Binding> m_explicit;
    std::map<std::string, std::vector<Binding>> m_wildcard;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
