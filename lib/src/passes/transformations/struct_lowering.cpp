// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/struct_lowering.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/misc/math.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// A constant unsized decimal literal.
AST::Node::Ptr make_const(std::uint64_t value, const AST::Node::Ptr &at)
{
    return std::make_shared<AST::IntConstN>(10, -1, true, Misc::Math::u64_to_mpz(value),
                                            at->get_filename(), at->get_line());
}

bool is_aggregate(const AST::DataType::Ptr &type)
{
    return type && (type->is_node_type(AST::NodeType::StructType) ||
                    type->is_node_type(AST::NodeType::UnionType));
}

} // namespace

int StructLowering::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        return process_definition(module->get_ports(), module->get_items(), node);
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        return process_definition(interface->get_ports(), interface->get_items(), node);
    }

    default: {
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret |= process(child, node);
        }
        return ret;
    }
    }
}

int StructLowering::process_definition(const AST::Port::ListPtr &ports,
                                       const AST::Node::ListPtr &items, const AST::Node::Ptr &node)
{
    m_scopes.emplace_back();
    int ret = 0;
    // Ports first: their decls are module-scope names, visible to the
    // whole body (like the header refinement of ADR-0009 §2).
    if(ports) {
        for(const AST::Port::Ptr &port : *ports) {
            if(port->get_decl()) {
                ret |= register_decl(port->get_decl());
            }
        }
    }
    if(ret == 0) {
        ret = process_items(items, node);
    }
    m_scopes.pop_back();
    return ret;
}

int StructLowering::scoped_items(const AST::Node::ListPtr &items, const AST::Node::Ptr &node)
{
    m_scopes.emplace_back();
    const int ret = process_items(items, node);
    m_scopes.pop_back();
    return ret;
}

int StructLowering::process_items(const AST::Node::ListPtr &items, const AST::Node::Ptr &node)
{
    if(!items) {
        return 0;
    }
    // Scope-level declarations bind scope-wide (module/generate/block
    // names resolve at elaboration), so registration precedes rewriting.
    for(const AST::Node::Ptr &item : *items) {
        if(item->is_node_category(AST::NodeType::Declaration)) {
            if(register_decl(AST::cast_to<AST::Declaration>(item))) {
                return 1;
            }
        }
    }
    for(const AST::Node::Ptr &item : *items) {
        if(rewrite(item, node, false)) {
            return 1;
        }
    }
    return 0;
}

int StructLowering::register_decl(const AST::Declaration::Ptr &decl)
{
    if(!is_aggregate(decl->get_type())) {
        return 0;
    }
    const auto &vec = lower_type(decl->get_type(), decl->get_name());
    if(!vec) {
        return 1;
    }
    decl->set_type(vec);
    return 0;
}

AST::DataType::Ptr StructLowering::lower_type(const AST::DataType::Ptr &type,
                                              const std::string &name)
{
    // A packed array of aggregates (`struct packed {..} [1:0] v`) would
    // need per-element member maps; reject rather than lower at the wrong
    // width.
    if(type->get_packed_dims() && !type->get_packed_dims()->empty()) {
        LOG_ERROR_N(type) << "'" << name << "': a packed array of aggregates is not supported";
        return nullptr;
    }

    Layout layout;
    layout.is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
    if(compute_layout(type, name, layout.width, layout.members)) {
        return nullptr;
    }

    // The declaration becomes its equivalent vector (§7.2.1/§7.3.1): a
    // logic [W-1:0], signed when the aggregate is declared signed.
    const auto &vec = std::make_shared<AST::LogicType>(type->get_filename(), type->get_line());
    if(layout.is_signed) {
        vec->set_signing(AST::DataType::SigningEnum::SIGNED);
    }
    const auto &range = std::make_shared<AST::RangeDim>(type->get_filename(), type->get_line());
    range->set_left(make_const(layout.width - 1, type));
    range->set_right(make_const(0, type));
    const auto &dims = std::make_shared<AST::Dimension::List>();
    dims->push_back(range);
    vec->set_packed_dims(dims);

    m_scopes.back()[name] = layout;
    return vec;
}

int StructLowering::compute_layout(const AST::DataType::Ptr &type, const std::string &decl_name,
                                   std::uint64_t &width, std::map<std::string, MemberInfo> &members)
{
    const bool is_union = type->is_node_type(AST::NodeType::UnionType);
    const bool is_packed = is_union ? AST::cast_to<AST::UnionType>(type)->get_is_packed()
                                    : AST::cast_to<AST::StructType>(type)->get_is_packed();
    if(is_union && AST::cast_to<AST::UnionType>(type)->get_is_tagged()) {
        // §7.3.2 gives a PACKED tagged union a standard bit layout (tag bits
        // plus the widest member, tag towards the MSBs), so it is lowerable by
        // the same vector mapping — merely unimplemented, like the unpacked
        // forms below. Unreachable today: `tagged` is not a scanner keyword,
        // so such a union never parses.
        LOG_ERROR_N(type) << "'" << decl_name
                          << "': a tagged union is not supported yet (IEEE 1800-2017 7.3.2)";
        return 1;
    }
    if(!is_packed) {
        // Not a language restriction in either case: the construct is legal
        // and synthesizable, it is this vector lowering that needs a defined
        // bit layout (§7.2.1) an unpacked aggregate does not have. What could
        // replace it differs, hence two messages: an unpacked STRUCT could be
        // split into one signal per member (ADR-0011 §5), while an unpacked
        // UNION is a single storage location read back through any member
        // type (§7.3) — splitting it would change behaviour, so no such
        // lowering is promised for it.
        if(is_union) {
            LOG_ERROR_N(type) << "'" << decl_name
                              << "': an unpacked union is not supported yet: it has no defined "
                                 "bit layout to lower to a vector, and its members share one "
                                 "storage location (IEEE 1800-2017 7.3), so they cannot be "
                                 "lowered to separate signals either";
        } else {
            LOG_ERROR_N(type) << "'" << decl_name
                              << "': an unpacked struct is not supported yet: it has no defined "
                                 "bit layout to lower to a vector (IEEE 1800-2017 7.2), and "
                                 "lowering it member by member is not implemented";
        }
        return 1;
    }

    const AST::Member::ListPtr member_list =
        is_union ? AST::cast_to<AST::UnionType>(type)->get_members()
                 : AST::cast_to<AST::StructType>(type)->get_members();
    if(!member_list || member_list->empty()) {
        LOG_ERROR_N(type) << "'" << decl_name << "': a packed struct/union needs members";
        return 1;
    }

    // Gather each member's width, then assign slices: a struct stacks them
    // first-member-at-MSBs (§7.2.1); a union overlays them all on [W-1:0],
    // every member the same width (§7.3.1).
    std::vector<std::pair<AST::Member::Ptr, MemberInfo>> infos;
    std::uint64_t total = 0;
    for(const AST::Member::Ptr &member : *member_list) {
        MemberInfo info;
        std::uint64_t w = 0;
        if(member_width(member, w, info)) {
            return 1;
        }
        info.msb = w; // width, repositioned below
        if(is_union) {
            if(total != 0 && w != total) {
                LOG_ERROR_N(member) << "packed union '" << decl_name
                                    << "': members have differing widths (IEEE 1800-2017 7.3.1)";
                return 1;
            }
            total = w;
        } else {
            total += w;
        }
        infos.emplace_back(member, info);
    }

    std::uint64_t remaining = total;
    for(auto &entry : infos) {
        const std::uint64_t w = entry.second.msb;
        if(is_union) {
            entry.second.msb = total - 1;
            entry.second.lsb = 0;
        } else {
            entry.second.msb = remaining - 1;
            entry.second.lsb = remaining - w;
            remaining -= w;
        }
        // Nested members were computed relative to their own aggregate;
        // shift them to absolute positions within the outer vector.
        std::vector<MemberInfo *> stack;
        for(auto &sub : entry.second.members) {
            stack.push_back(&sub.second);
        }
        while(!stack.empty()) {
            MemberInfo *info = stack.back();
            stack.pop_back();
            info->msb += entry.second.lsb;
            info->lsb += entry.second.lsb;
            for(auto &sub : info->members) {
                stack.push_back(&sub.second);
            }
        }
        members[entry.first->get_name()] = entry.second;
    }

    width = total;
    return 0;
}

int StructLowering::member_width(const AST::Member::Ptr &member, std::uint64_t &width,
                                 MemberInfo &info)
{
    if(member->get_unpacked_dims() && !member->get_unpacked_dims()->empty()) {
        LOG_ERROR_N(member) << "member '" << member->get_name()
                            << "': unpacked dims are not legal in a packed aggregate";
        return 1;
    }

    const auto &type = member->get_type();
    if(!type) {
        LOG_ERROR_N(member) << "member '" << member->get_name() << "' has no type";
        return 1;
    }

    // A nested packed aggregate recurses; its slice bounds are made
    // absolute by the caller. Selects address its sub-members, never the
    // aggregate as an array.
    if(is_aggregate(type)) {
        if(type->get_packed_dims() && !type->get_packed_dims()->empty()) {
            LOG_ERROR_N(member) << "member '" << member->get_name()
                                << "': a packed array of aggregates is not supported";
            return 1;
        }
        info.is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        info.sel = SelKind::none;
        return compute_layout(type, member->get_name(), width, info.members);
    }

    std::uint64_t base = 1;
    if(!Analysis::Dimensions::integral_base(type, base, info.is_signed)) {
        LOG_ERROR_N(member) << "member '" << member->get_name()
                            << "': not an integral type (IEEE 1800-2017 7.2.1)";
        return 1;
    }

    // Selects into the member fold against its DECLARED range (§7.4.2):
    // record it for single-range vector shapes; an atom addresses as
    // [W-1:0]. A multi-dim member cannot fold a select (an element select
    // is not a bit select) — accessing it whole stays fine.
    const auto &pdims = type->get_packed_dims();
    const std::size_t ndims = pdims ? pdims->size() : 0;
    if(ndims == 0) {
        width = base;
        info.sel = SelKind::descending;
        info.range_left = static_cast<std::int64_t>(base) - 1;
        info.range_right = 0;
        return 0;
    }

    Analysis::Dimensions::DimList dims;
    if(!Analysis::Dimensions::extract_arrays(pdims, Analysis::Dimensions::Packing::packed, dims)) {
        LOG_ERROR_N(member) << "struct member '" << member->get_name()
                            << "': the width is not constant";
        return 1;
    }
    width = base * dims.packed_width();

    if(ndims == 1 && base == 1) {
        Analysis::Dimensions::DimInfo dim;
        if(!Analysis::Dimensions::extract_dimension(pdims->front(),
                                                    Analysis::Dimensions::Packing::packed, dim)) {
            LOG_ERROR_N(member) << "struct member '" << member->get_name()
                                << "': the width is not constant";
            return 1;
        }
        // DimInfo keeps the written bounds: msb = left, lsb = right, and
        // is_big = left > right — i.e. a conventional descending range.
        info.sel = dim.is_big ? SelKind::descending : SelKind::ascending;
        info.range_left = dim.msb;
        info.range_right = dim.lsb;
    } else {
        info.sel = SelKind::none;
    }
    return 0;
}

int StructLowering::rewrite(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                            bool in_lvalue)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Identifier:
        return rewrite_access(AST::cast_to<AST::Identifier>(node), parent, in_lvalue);

    case AST::NodeType::Lvalue: {
        const AST::Node::ListPtr children = node->get_children();
        int ret = 0;
        for(const AST::Node::Ptr &child : *children) {
            ret |= rewrite(child, node, true);
        }
        return ret;
    }

    // Select nodes: the selected variable inherits the context, but the
    // index/bound subexpressions are rvalue even under an assignment
    // target (`mem[base + s.off] = x` reads s.off).
    case AST::NodeType::Pointer: {
        const auto &ptr = AST::cast_to<AST::Pointer>(node);
        int ret = rewrite(ptr->get_ptr(), node, false);
        ret |= rewrite(ptr->get_var(), node, in_lvalue);
        return ret;
    }

    case AST::NodeType::Partselect: {
        const auto &sel = AST::cast_to<AST::Partselect>(node);
        int ret = rewrite(sel->get_msb(), node, false);
        ret |= rewrite(sel->get_lsb(), node, false);
        ret |= rewrite(sel->get_var(), node, in_lvalue);
        return ret;
    }

    case AST::NodeType::PartselectPlusIndexed: {
        const auto &sel = AST::cast_to<AST::PartselectPlusIndexed>(node);
        int ret = rewrite(sel->get_index(), node, false);
        ret |= rewrite(sel->get_size(), node, false);
        ret |= rewrite(sel->get_var(), node, in_lvalue);
        return ret;
    }

    case AST::NodeType::PartselectMinusIndexed: {
        const auto &sel = AST::cast_to<AST::PartselectMinusIndexed>(node);
        int ret = rewrite(sel->get_index(), node, false);
        ret |= rewrite(sel->get_size(), node, false);
        ret |= rewrite(sel->get_var(), node, in_lvalue);
        return ret;
    }

    // Nested scopes: their declarations shadow enclosing bindings.
    case AST::NodeType::Block:
        return scoped_items(AST::cast_to<AST::Block>(node)->get_statements(), node);

    case AST::NodeType::ParallelBlock:
        return scoped_items(AST::cast_to<AST::ParallelBlock>(node)->get_statements(), node);

    case AST::NodeType::GenerateStatement:
        return scoped_items(AST::cast_to<AST::GenerateStatement>(node)->get_items(), node);

    case AST::NodeType::Function: {
        const auto &function = AST::cast_to<AST::Function>(node);
        return process_subroutine(function->get_args(), function->get_statements(), node, function);
    }

    case AST::NodeType::Task: {
        const auto &task = AST::cast_to<AST::Task>(node);
        return process_subroutine(task->get_args(), task->get_statements(), node, nullptr);
    }

    default: {
        const AST::Node::ListPtr children = node->get_children();
        int ret = 0;
        for(const AST::Node::Ptr &child : *children) {
            ret |= rewrite(child, node, in_lvalue);
        }
        return ret;
    }
    }
}

int StructLowering::process_subroutine(const AST::Arg::ListPtr &args,
                                       const AST::Node::ListPtr &statements,
                                       const AST::Node::Ptr &node,
                                       const AST::Function::Ptr &function)
{
    m_scopes.emplace_back();
    int ret = 0;
    // A function's aggregate return type lowers like a declaration: the
    // function name is the implicit return variable (§13.4), so
    // `fname.member = ...` resolves against its layout.
    if(function && is_aggregate(function->get_return_type())) {
        const auto &vec = lower_type(function->get_return_type(), function->get_name());
        if(!vec) {
            m_scopes.pop_back();
            return 1;
        }
        function->set_return_type(vec);
    }
    if(args) {
        for(const AST::Arg::Ptr &arg : *args) {
            ret |= register_decl(arg);
        }
    }
    if(ret == 0) {
        ret = process_items(statements, node);
    }
    m_scopes.pop_back();
    return ret;
}

int StructLowering::rewrite_access(const AST::Identifier::Ptr &ident, const AST::Node::Ptr &parent,
                                   bool in_lvalue)
{
    const auto &hier = ident->get_hier();
    const auto &labels = hier ? hier->get_labellist() : nullptr;
    if(!labels || labels->empty()) {
        return 0;
    }

    const AST::HierLabel::Ptr &root = labels->front();
    const Layout *layout = lookup(root->get_name());
    if(!layout) {
        // Not a lowered declaration: an interface access or a genuine
        // hierarchical reference — later passes own it.
        return 0;
    }

    // Resolve the member path: the labels beyond the root, then the
    // identifier's own name.
    const MemberInfo *info = nullptr;
    const std::map<std::string, MemberInfo> *level = &layout->members;
    auto it = labels->begin();
    for(++it;; ++it) {
        const bool last = (it == labels->end());
        const std::string &name = last ? ident->get_name() : (*it)->get_name();
        if(!last && (*it)->get_loop()) {
            LOG_ERROR_N(ident) << "'" << root->get_name() << "." << name
                               << "': an indexed intermediate member is not legal in a "
                               << "packed aggregate";
            return 1;
        }
        const auto found = level->find(name);
        if(found == level->end()) {
            LOG_ERROR_N(ident) << "'" << root->get_name() << "' has no member '" << name << "'";
            return 1;
        }
        info = &found->second;
        level = &found->second.members;
        if(last) {
            break;
        }
    }

    // The access base: the bare declaration, through the root's index for
    // an array-of-aggregates element (`bank[i].f`).
    AST::Node::Ptr base = std::make_shared<AST::Identifier>(
        nullptr, nullptr, root->get_name(), ident->get_filename(), ident->get_line());
    if(root->get_loop()) {
        base = std::make_shared<AST::Pointer>(root->get_loop(), base, ident->get_filename(),
                                              ident->get_line());
    }

    if(!parent) {
        LOG_ERROR_N(ident) << "member access has no enclosing node";
        return 1;
    }

    // A directly enclosing select folds into the member offset instead of
    // stacking on the emitted part-select. The written index names a bit
    // of the member's DECLARED range (§7.4.2): a descending [l:r] member
    // maps bit i to vector bit lsb+(i-r); an ascending [l:r] member maps
    // bit i to vector bit msb-(i-l).
    const bool sel_parent =
        (parent->is_node_type(AST::NodeType::Pointer) &&
         AST::cast_to<AST::Pointer>(parent)->get_var() == ident) ||
        (parent->is_node_type(AST::NodeType::Partselect) &&
         AST::cast_to<AST::Partselect>(parent)->get_var() == ident) ||
        (parent->is_node_type(AST::NodeType::PartselectPlusIndexed) &&
         AST::cast_to<AST::PartselectPlusIndexed>(parent)->get_var() == ident) ||
        (parent->is_node_type(AST::NodeType::PartselectMinusIndexed) &&
         AST::cast_to<AST::PartselectMinusIndexed>(parent)->get_var() == ident);

    if(sel_parent) {
        if(info->sel == SelKind::none) {
            LOG_ERROR_N(ident) << "'" << root->get_name() << "." << ident->get_name()
                               << "': a select into this member is not supported";
            return 1;
        }
        const auto &normalize = [&](const AST::Node::Ptr &sel) -> AST::Node::Ptr {
            if(info->sel == SelKind::ascending) {
                // vector bit = msb - (i - left) = (msb + left) - i
                const std::uint64_t kappa =
                    info->msb + static_cast<std::uint64_t>(info->range_left);
                return std::make_shared<AST::Minus>(make_const(kappa, ident), sel,
                                                    ident->get_filename(), ident->get_line());
            }
            // vector bit = lsb + (i - right)
            const std::int64_t delta = static_cast<std::int64_t>(info->lsb) - info->range_right;
            if(delta == 0) {
                return sel;
            }
            if(delta > 0) {
                return std::make_shared<AST::Plus>(
                    make_const(static_cast<std::uint64_t>(delta), ident), sel,
                    ident->get_filename(), ident->get_line());
            }
            return std::make_shared<AST::Minus>(
                sel, make_const(static_cast<std::uint64_t>(-delta), ident), ident->get_filename(),
                ident->get_line());
        };
        if(parent->is_node_type(AST::NodeType::Pointer)) {
            const auto &ptr = AST::cast_to<AST::Pointer>(parent);
            ptr->set_ptr(normalize(ptr->get_ptr()));
            ptr->set_var(base);
            return 0;
        }
        if(parent->is_node_type(AST::NodeType::Partselect)) {
            const auto &sel = AST::cast_to<AST::Partselect>(parent);
            sel->set_msb(normalize(sel->get_msb()));
            sel->set_lsb(normalize(sel->get_lsb()));
            sel->set_var(base);
            return 0;
        }
        // Indexed part-selects sweep upward from the base index; on an
        // ascending member the sweep direction inverts — reject rather
        // than mis-map.
        if(info->sel == SelKind::ascending) {
            LOG_ERROR_N(ident) << "'" << root->get_name() << "." << ident->get_name()
                               << "': an indexed part-select on an ascending member is not "
                               << "supported";
            return 1;
        }
        if(parent->is_node_type(AST::NodeType::PartselectPlusIndexed)) {
            const auto &sel = AST::cast_to<AST::PartselectPlusIndexed>(parent);
            sel->set_index(normalize(sel->get_index()));
            sel->set_var(base);
            return 0;
        }
        const auto &sel = AST::cast_to<AST::PartselectMinusIndexed>(parent);
        sel->set_index(normalize(sel->get_index()));
        sel->set_var(base);
        return 0;
    }

    AST::Node::Ptr access = std::make_shared<AST::Partselect>(
        make_const(info->msb, ident), make_const(info->lsb, ident), base, ident->get_filename(),
        ident->get_line());
    // A signed member keeps its signedness in expression position
    // (§11.5.1 makes a part-select unsigned); assignment targets carry no
    // signedness semantics.
    if(info->is_signed && !in_lvalue) {
        access = std::make_shared<AST::SigningCast>(access, AST::SigningCast::SigningEnum::SIGNED,
                                                    ident->get_filename(), ident->get_line());
    }
    parent->replace(ident, access);
    return 0;
}

const StructLowering::Layout *StructLowering::lookup(const std::string &name) const
{
    for(auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        const auto found = it->find(name);
        if(found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
