// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_STRUCT_LOWERING
#define VERIPARSE_PASSES_TRANSFORMATIONS_STRUCT_LOWERING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <cstdint>
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
 * @brief Lower packed struct/union declaration types to their equivalent
 * packed vector and rewrite member accesses to part-selects (ADR-0011).
 *
 * A packed structure is a vector whose first member occupies the most
 * significant bits (IEEE 1800-2017 §7.2.1); a packed union overlays every
 * member on the full width, all members being the same size (§7.3.1). The
 * pass computes each declaration's member layout, replaces the declaration
 * type with a logic vector, and rewrites `s.f` accesses — Identifier hier
 * paths (ADR-0008 §4.3) — to constant part-selects, folding a directly
 * enclosing bit/part-select into the member offset. A signed member read
 * in expression position re-wraps in signed'().
 *
 * Must run after TypedefInliner: declaration types are then concrete
 * (struct typedefs and type parameters substituted, enum and named member
 * types lowered) and parameter-dependent member widths are folded.
 * Identifiers whose root name binds to no lowered declaration are left
 * untouched — interface accesses and hierarchical references resolve in
 * later passes.
 */
class StructLowering : public TransformationBase
{
private:
    /// A member's slice of the lowered vector (absolute bounds), plus its
    /// own sub-members when the member is itself a packed struct/union.
    struct MemberInfo
    {
        std::uint64_t msb{0};
        std::uint64_t lsb{0};
        bool is_signed{false};
        std::map<std::string, MemberInfo> members;
    };

    /// The layout of one lowered declaration.
    struct Layout
    {
        std::uint64_t width{0};
        bool is_signed{false};
        std::map<std::string, MemberInfo> members;
    };

    using Scope = std::map<std::string, Layout>;

    /**
     * @return zero on success
     */
    int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

    /**
     * @brief Register the struct/union-typed declarations of one scope's
     * item list, then rewrite the member accesses within it.
     */
    int process_items(const AST::Node::ListPtr &items, const AST::Node::Ptr &node);

    /**
     * @brief Lower one declaration when its type is a packed struct/union;
     * registers the layout under the declaration's name.
     */
    int register_decl(const AST::Declaration::Ptr &decl);

    /**
     * @brief Compute the member layout of a packed struct/union type.
     * @return zero on success
     */
    int compute_layout(const AST::DataType::Ptr &type, const std::string &decl_name,
                       std::uint64_t &width, std::map<std::string, MemberInfo> &members);

    /**
     * @brief The packed width and signedness of an integral member type.
     * @return zero on success
     */
    int member_width(const AST::Member::Ptr &member, std::uint64_t &width, bool &is_signed,
                     std::map<std::string, MemberInfo> &members);

    /**
     * @brief Rewrite member accesses in a subtree; in_lvalue suppresses the
     * signed'() re-wrap under assignment targets.
     */
    int rewrite(const AST::Node::Ptr &node, const AST::Node::Ptr &parent, bool in_lvalue);

    /**
     * @brief Rewrite one hier identifier to a part-select when its root
     * binds to a lowered declaration. Folds a directly enclosing
     * bit/part-select into the member offset.
     */
    int rewrite_access(const AST::Identifier::Ptr &ident, const AST::Node::Ptr &parent,
                       bool in_lvalue);

    /**
     * @brief Subroutine bodies: args register against the body scope.
     */
    int process_subroutine(const AST::Arg::ListPtr &args, const AST::Node::ListPtr &statements,
                           const AST::Node::Ptr &node);

    const Layout *lookup(const std::string &name) const;

private:
    std::vector<Scope> m_scopes;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
