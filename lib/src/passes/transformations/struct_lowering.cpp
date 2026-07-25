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

/// The product of a type's packed dims; false when a dim is not constant.
bool packed_dims_width(const AST::DataType::Ptr &type, std::uint64_t &width)
{
    width = 1;
    if(type->get_packed_dims()) {
        for(const AST::Dimension::Ptr &dim : *type->get_packed_dims()) {
            Analysis::Dimensions::DimInfo info;
            if(!Analysis::Dimensions::extract_dimension(dim, Analysis::Dimensions::Packing::packed,
                                                        info)) {
                return false;
            }
            width *= info.width;
        }
    }
    return true;
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
        m_scopes.emplace_back();
        int ret = 0;
        // Ports first: their decls are module-scope names, visible to the
        // whole body (like the header refinement of ADR-0009 §2).
        if(module->get_ports()) {
            for(const AST::Port::Ptr &port : *module->get_ports()) {
                if(port->get_decl()) {
                    ret |= register_decl(port->get_decl());
                }
            }
        }
        if(ret == 0) {
            ret = process_items(module->get_items(), node);
        }
        m_scopes.pop_back();
        return ret;
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        m_scopes.emplace_back();
        int ret = 0;
        if(interface->get_ports()) {
            for(const AST::Port::Ptr &port : *interface->get_ports()) {
                if(port->get_decl()) {
                    ret |= register_decl(port->get_decl());
                }
            }
        }
        if(ret == 0) {
            ret = process_items(interface->get_items(), node);
        }
        m_scopes.pop_back();
        return ret;
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
    const auto &type = decl->get_type();
    if(!type || (!type->is_node_type(AST::NodeType::StructType) &&
                 !type->is_node_type(AST::NodeType::UnionType))) {
        return 0;
    }

    Layout layout;
    layout.is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
    if(compute_layout(type, decl->get_name(), layout.width, layout.members)) {
        return 1;
    }

    // The declaration becomes its equivalent vector (§7.2.1/§7.3.1): a
    // logic [W-1:0], signed when the aggregate is declared signed.
    const auto &vec = std::make_shared<AST::LogicType>(type->get_filename(), type->get_line());
    if(layout.is_signed) {
        vec->set_signing(AST::DataType::SigningEnum::SIGNED);
    }
    const auto &range = std::make_shared<AST::RangeDim>(type->get_filename(), type->get_line());
    range->set_left(make_const(layout.width - 1, decl));
    range->set_right(make_const(0, decl));
    const auto &dims = std::make_shared<AST::Dimension::List>();
    dims->push_back(range);
    vec->set_packed_dims(dims);
    decl->set_type(vec);

    m_scopes.back()[decl->get_name()] = layout;
    return 0;
}

int StructLowering::compute_layout(const AST::DataType::Ptr &type, const std::string &decl_name,
                                   std::uint64_t &width, std::map<std::string, MemberInfo> &members)
{
    const bool is_union = type->is_node_type(AST::NodeType::UnionType);
    const bool is_packed = is_union ? AST::cast_to<AST::UnionType>(type)->get_is_packed()
                                    : AST::cast_to<AST::StructType>(type)->get_is_packed();
    if(is_union && AST::cast_to<AST::UnionType>(type)->get_is_tagged()) {
        LOG_ERROR_N(type) << "tagged union is not synthesizable (IEEE 1800-2017 7.3.2)";
        return 1;
    }
    if(!is_packed) {
        LOG_ERROR_N(type) << "unpacked struct/union is not synthesizable here";
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
        if(member_width(member, w, info.is_signed, info.members)) {
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
                                 bool &is_signed, std::map<std::string, MemberInfo> &members)
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
    // absolute by the caller.
    if(type->is_node_type(AST::NodeType::StructType) ||
       type->is_node_type(AST::NodeType::UnionType)) {
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        return compute_layout(type, member->get_name(), width, members);
    }

    std::uint64_t base = 1;
    switch(type->get_node_type()) {
    case AST::NodeType::LogicType:
    case AST::NodeType::RegType:
    case AST::NodeType::BitType:
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        break;
    case AST::NodeType::ByteType:
        base = 8;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::ShortintType:
        base = 16;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::IntType:
    case AST::NodeType::IntegerType:
        base = 32;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::LongintType:
        base = 64;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::TimeType:
        base = 64;
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        break;
    default:
        LOG_ERROR_N(member) << "member '" << member->get_name()
                            << "': not an integral type (IEEE 1800-2017 7.2.1)";
        return 1;
    }

    std::uint64_t dims = 1;
    if(!packed_dims_width(type, dims)) {
        LOG_ERROR_N(member) << "struct member '" << member->get_name()
                            << "': the width is not constant";
        return 1;
    }
    width = base * dims;
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

    // Nested scopes: their declarations shadow enclosing bindings.
    case AST::NodeType::Block:
        m_scopes.emplace_back();
        {
            const int ret = process_items(AST::cast_to<AST::Block>(node)->get_statements(), node);
            m_scopes.pop_back();
            return ret;
        }

    case AST::NodeType::ParallelBlock:
        m_scopes.emplace_back();
        {
            const int ret =
                process_items(AST::cast_to<AST::ParallelBlock>(node)->get_statements(), node);
            m_scopes.pop_back();
            return ret;
        }

    case AST::NodeType::GenerateStatement:
        m_scopes.emplace_back();
        {
            const int ret =
                process_items(AST::cast_to<AST::GenerateStatement>(node)->get_items(), node);
            m_scopes.pop_back();
            return ret;
        }

    case AST::NodeType::Function: {
        const auto &function = AST::cast_to<AST::Function>(node);
        return process_subroutine(function->get_args(), function->get_statements(), node);
    }

    case AST::NodeType::Task: {
        const auto &task = AST::cast_to<AST::Task>(node);
        return process_subroutine(task->get_args(), task->get_statements(), node);
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
                                       const AST::Node::Ptr &node)
{
    m_scopes.emplace_back();
    int ret = 0;
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
    // stacking on the emitted part-select (`s.f[i]` is bit lsb+i of s).
    const auto &offset = [&](const AST::Node::Ptr &sel) -> AST::Node::Ptr {
        if(info->lsb == 0) {
            return sel;
        }
        return std::make_shared<AST::Plus>(make_const(info->lsb, ident), sel, ident->get_filename(),
                                           ident->get_line());
    };
    if(parent->is_node_type(AST::NodeType::Pointer) &&
       AST::cast_to<AST::Pointer>(parent)->get_var() == ident) {
        const auto &ptr = AST::cast_to<AST::Pointer>(parent);
        ptr->set_ptr(offset(ptr->get_ptr()));
        ptr->set_var(base);
        return 0;
    }
    if(parent->is_node_type(AST::NodeType::Partselect) &&
       AST::cast_to<AST::Partselect>(parent)->get_var() == ident) {
        const auto &sel = AST::cast_to<AST::Partselect>(parent);
        sel->set_msb(offset(sel->get_msb()));
        sel->set_lsb(offset(sel->get_lsb()));
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
