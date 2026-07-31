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
    /// How a bit/part select into a member maps to the lowered vector:
    /// against a descending or ascending declared range, or not at all (a
    /// nested aggregate or a multi-packed-dim member).
    enum class SelKind
    {
        none,
        descending,
        ascending
    };

    /// A member's slice of the lowered vector (absolute bounds), its
    /// declared range for select normalization (§7.4.2), plus its own
    /// sub-members when the member is itself a packed struct/union.
    struct MemberInfo
    {
        std::uint64_t msb{0};
        std::uint64_t lsb{0};
        bool is_signed{false};
        /// Members overlay one another (§7.3.1) instead of stacking, so
        /// they cannot be concatenated from an assignment pattern.
        bool is_union{false};
        SelKind sel{SelKind::none};
        std::int64_t range_left{0};
        std::int64_t range_right{0};
        std::map<std::string, MemberInfo> members;
    };

    /// The layout of one lowered declaration.
    struct Layout
    {
        std::uint64_t width{0};
        bool is_signed{false};
        /// As MemberInfo::is_union, for the declaration's own type.
        bool is_union{false};
        /// The declaration carries unpacked dimensions: an assignment
        /// pattern over it gives one value per ELEMENT, not per member.
        bool is_array{false};
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
     * @brief One module/interface definition: ports register into the
     * scope first (module-wide names), then the body is processed.
     */
    int process_definition(const AST::Port::ListPtr &ports, const AST::Node::ListPtr &items,
                           const AST::Node::Ptr &node);

    /**
     * @brief Process an item list in a fresh nested scope.
     */
    int scoped_items(const AST::Node::ListPtr &items, const AST::Node::Ptr &node);

    /**
     * @brief Lower one declaration when its type is a packed struct/union;
     * registers the layout under the declaration's name.
     */
    int register_decl(const AST::Declaration::Ptr &decl);

    /**
     * @brief Lower an aggregate type to its vector: computes the layout,
     * registers it under `name`, and returns the replacement type (null on
     * error).
     */
    AST::DataType::Ptr lower_type(const AST::DataType::Ptr &type, const std::string &name);

    /**
     * @brief Compute the member layout of a packed struct/union type.
     * @return zero on success
     */
    int compute_layout(const AST::DataType::Ptr &type, const std::string &decl_name,
                       std::uint64_t &width, std::map<std::string, MemberInfo> &members);

    /**
     * @brief The packed width of a member plus its select-normalization
     * info (declared range) and sub-members.
     * @return zero on success
     */
    int member_width(const AST::Member::Ptr &member, std::uint64_t &width, MemberInfo &info);

    /**
     * @brief Rewrite member accesses in a subtree; in_lvalue suppresses the
     * signed'() re-wrap under assignment targets.
     */
    int rewrite(const AST::Node::Ptr &node, const AST::Node::Ptr &parent, bool in_lvalue);

    /// Lower an assignment pattern assigned to a lowered aggregate (§10.9)
    /// into the concatenation its target vector expects: members ordered
    /// first-at-the-MSBs (§7.2.1). Positional, member-keyed and
    /// `default:` forms are handled; anything else is rejected rather than
    /// guessed. Does nothing when @p assign's target is not a lowered
    /// aggregate.
    int lower_assignment_pattern(const AST::Node::Ptr &assign);

    /// Same, for a pattern in a declaration's initializer.
    int lower_declaration_pattern(const AST::Var::Ptr &var);

    /// Members of one aggregate, first-at-the-MSBs (§7.2.1). The map is
    /// keyed by name, so descending msb recovers the declaration order a
    /// positional pattern follows.
    static std::vector<std::pair<std::string, const MemberInfo *>>
    ordered_members(const std::map<std::string, MemberInfo> &members);

    /// Lower @p pattern against the member set it targets, returning the
    /// equivalent concatenation (members first-at-the-MSBs, §7.2.1) or null
    /// after reporting why it cannot be lowered. @p path names the target
    /// for diagnostics.
    AST::Node::Ptr lower_pattern_over(const AST::AssignmentPattern::Ptr &pattern,
                                      const std::map<std::string, MemberInfo> &members,
                                      const std::string &path);

    /// One member's value: a nested pattern lowers against that member's own
    /// layout, anything else is sized to the member (§10.9 assigns, whereas
    /// a concat element keeps its own width).
    AST::Node::Ptr lower_member_value(const AST::Node::Ptr &value, const MemberInfo &info,
                                      const std::string &path);

    /// `default:` reaching a member: applied recursively to each member of a
    /// substructure (§10.9.2), sized to the member otherwise.
    AST::Node::Ptr apply_default(const AST::Node::Ptr &value, const MemberInfo &info,
                                 const std::string &path);

    /// The member set an assignment-pattern target denotes: the declaration's
    /// own for a bare name, or a nested member's for a hierarchical one.
    /// Null (without diagnostic) when the target is not a lowered aggregate.
    const std::map<std::string, MemberInfo> *pattern_target(const AST::Identifier::Ptr &target,
                                                            std::string &path, bool &rejected);

    /**
     * @brief Rewrite one hier identifier to a part-select when its root
     * binds to a lowered declaration. Folds a directly enclosing
     * bit/part-select into the member offset.
     */
    int rewrite_access(const AST::Identifier::Ptr &ident, const AST::Node::Ptr &parent,
                       bool in_lvalue);

    /**
     * @brief Subroutine bodies: args register against the body scope; a
     * function's aggregate return type lowers and registers under the
     * function's own name (the implicit return variable).
     */
    int process_subroutine(const AST::Arg::ListPtr &args, const AST::Node::ListPtr &statements,
                           const AST::Node::Ptr &node, const AST::Function::Ptr &function);

    const Layout *lookup(const std::string &name) const;

private:
    std::vector<Scope> m_scopes;
};

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
