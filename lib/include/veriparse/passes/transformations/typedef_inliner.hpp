// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TYPEDEF_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_TYPEDEF_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

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
 * @brief Substitute every user-defined type name with its underlying data
 * type and drop the typedef declarations (ADR-0009).
 *
 * Must run after EnumInliner: enum references are already constants, so an
 * enum-typed declaration lowers to the enum's base type without losing
 * information, and parameter-dependent typedef dims are already folded.
 *
 * Typedef bindings are lexical: a nested scope (generate region, begin/end
 * or fork/join block, function or task body) shadows an enclosing typedef
 * of the same name. Within
 * the body, a typedef must precede its references (IEEE 1800-2017 §6.18);
 * header ports and parameters resolve against the whole module scope, since
 * package- and unit-scope typedefs are spliced at the head of the body by
 * PackageInliner and must stay visible to the header.
 *
 * After the pass the module contains no Typedef item and no NamedType
 * reference.
 */
class TypedefInliner : public TransformationBase
{
private:
    /// A resolved alias: the substituted underlying type, plus the typedef's
    /// own unpacked dims (an array typedef, §6.18) merged into the use-site
    /// declaration. `complete` is false for a pending forward typedef.
    struct Alias
    {
        AST::DataType::Ptr type;
        AST::Dimension::ListPtr unpacked_dims;
        bool complete{false};
    };

    using Scope = std::map<std::string, Alias>;

    /**
     * @return zero on success
     */
    int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Register the typedefs of one scope's item list in declaration
     * order, substituting the other items in place, then drop the typedefs.
     * @return zero on success
     */
    int process_items(const AST::Node::ListPtr &items);

    /**
     * @brief Resolve a typedef's own aliased type and bind it in the current
     * scope. A forward typedef (null type) binds incomplete until its
     * completing definition.
     * @return zero on success
     */
    int register_typedef(const AST::Typedef::Ptr &tdef);

    /**
     * @brief Recursively substitute NamedType references in a subtree,
     * entering nested scopes.
     * @return zero on success
     */
    int substitute(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

    /**
     * @brief Replace one NamedType reference with a clone of its alias,
     * merging an array typedef's dims into the use-site declaration.
     * @return zero on success
     */
    int substitute_named_type(const AST::NamedType::Ptr &named, const AST::Node::Ptr &parent);

    /**
     * @brief Lower a cast to a typedef into a SizeCast of the alias's packed
     * width (IEEE 1800-2017 §6.24.1) — a TypeCast target has no legal
     * rendering for a non-named type.
     * @return zero on success
     */
    int substitute_typedef_cast(const AST::TypeCast::Ptr &cast, const AST::Node::Ptr &parent);

    /**
     * @brief The innermost binding of a name, or nullptr.
     */
    const Alias *lookup(const std::string &name) const;

private:
    std::vector<Scope> m_scopes;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
