// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/typedef_inliner.hpp>

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

/// The concrete type an enum decl type lowers to: its base data type, or the
/// §6.19 default `int` when no base is written. Enum item references are
/// already constants (EnumInliner runs first), so the items are dead weight
/// that would re-declare the enumerators at every flattened splice.
AST::DataType::Ptr lowered_enum(const AST::EnumType::Ptr &etype)
{
    const auto &base = etype->get_base();
    if(base) {
        return AST::cast_to<AST::DataType>(base->clone());
    }
    return std::make_shared<AST::IntType>(etype->get_filename(), etype->get_line());
}

} // namespace

int TypedefInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        m_scopes.emplace_back();
        // The body first: it registers the module-scope typedefs, including
        // the package/unit-scope ones PackageInliner splices at its head.
        // Header ports and parameters then resolve against the whole module
        // scope — they lexically precede the body, but the spliced typedefs
        // must stay visible to them.
        int ret = process_items(module->get_items());
        if(ret == 0 && module->get_ports()) {
            for(const AST::Port::Ptr &port : *module->get_ports()) {
                ret |= substitute(port, node);
            }
        }
        if(ret == 0 && module->get_params()) {
            for(const AST::Declaration::Ptr &param : *module->get_params()) {
                ret |= substitute(param, node);
            }
        }
        m_scopes.pop_back();
        return ret;
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        m_scopes.emplace_back();
        int ret = process_items(interface->get_items());
        if(ret == 0 && interface->get_ports()) {
            for(const AST::Port::Ptr &port : *interface->get_ports()) {
                ret |= substitute(port, node);
            }
        }
        if(ret == 0 && interface->get_params()) {
            for(const AST::Declaration::Ptr &param : *interface->get_params()) {
                ret |= substitute(param, node);
            }
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

int TypedefInliner::process_items(const AST::Node::ListPtr &items)
{
    if(!items) {
        return 0;
    }

    // Declaration order matters within a body (§6.18): a typedef binds from
    // its item onward, so a body reference to a later typedef fails the
    // lookup below.
    for(auto it = items->begin(); it != items->end();) {
        if((*it)->is_node_type(AST::NodeType::Typedef)) {
            if(register_typedef(AST::cast_to<AST::Typedef>(*it))) {
                return 1;
            }
            it = items->erase(it);
        } else {
            if(substitute(*it, nullptr)) {
                return 1;
            }
            ++it;
        }
    }
    return 0;
}

int TypedefInliner::register_typedef(const AST::Typedef::Ptr &tdef)
{
    Scope &scope = m_scopes.back();
    const std::string &name = tdef->get_name();

    if(!tdef->get_type()) {
        // Forward typedef: bind the name incomplete; the completing
        // definition overwrites it. Referencing it before then is an error
        // (substitute_named_type).
        scope.emplace(name, Alias{});
        return 0;
    }

    // Resolve the aliased type first, so chains collapse eagerly and every
    // binding holds a concrete type. The substitution also merges a chained
    // array typedef's dims onto this typedef.
    if(substitute(tdef->get_type(), tdef)) {
        return 1;
    }

    const auto it = scope.find(name);
    if(it != scope.end() && it->second.complete) {
        LOG_ERROR_N(tdef) << "typedef '" << name << "' is already declared in this scope";
        return 1;
    }

    Alias alias;
    alias.type = tdef->get_type();
    alias.unpacked_dims = tdef->get_unpacked_dims();
    alias.complete = true;
    scope[name] = alias;
    return 0;
}

int TypedefInliner::substitute(const AST::Node::Ptr &node, const AST::Node::Ptr &parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::NamedType:
        return substitute_named_type(AST::cast_to<AST::NamedType>(node), parent);

    case AST::NodeType::TypeCast: {
        // A cast to a typedef of a packed vector type is a width conversion
        // (§6.24.1): it lowers to a SizeCast of the alias's packed width —
        // a TypeCast target has no legal rendering for a non-named type.
        const auto &cast = AST::cast_to<AST::TypeCast>(node);
        if(substitute(cast->get_expr(), node)) {
            return 1;
        }
        const auto &target = cast->get_target();
        if(!target || !target->is_node_type(AST::NodeType::NamedType)) {
            return substitute(target, node);
        }
        return substitute_typedef_cast(cast, parent);
    }

    case AST::NodeType::EnumType: {
        // A declaration's enum type lowers to its base (ADR-0009 §4): the
        // items are dead weight once EnumInliner has inlined every
        // enumerator reference — kept, they would re-declare the enumerators
        // at each flattened splice. The base may itself be an alias
        // (`enum nib_t {..}`), so it substitutes first.
        const auto &etype = AST::cast_to<AST::EnumType>(node);
        if(etype->get_base() && substitute(etype->get_base(), node)) {
            return 1;
        }
        if(parent && parent->is_node_category(AST::NodeType::Declaration) &&
           AST::cast_to<AST::Declaration>(parent)->get_type() == node) {
            AST::cast_to<AST::Declaration>(parent)->set_type(lowered_enum(etype));
        }
        return 0;
    }

    case AST::NodeType::ParamArg: {
        // An instance type actual written as a bare identifier parses as an
        // expression. One name cannot denote both a value and a type in a
        // scope (§3.13), so an identifier that resolves to a typedef alias
        // here — the instantiating scope, where §23.10 evaluates actuals —
        // IS a type actual: rewrite it to the alias's concrete type. Any
        // other identifier is a value actual for the usual passes.
        const auto &parg = AST::cast_to<AST::ParamArg>(node);
        const auto &value = parg->get_value();
        if(value && value->is_node_type(AST::NodeType::Identifier)) {
            const auto &ident = AST::cast_to<AST::Identifier>(value);
            if((!ident->get_scope() || ident->get_scope()->empty()) && !ident->get_hier()) {
                const Alias *alias = lookup(ident->get_name());
                if(alias) {
                    if(!alias->complete) {
                        LOG_ERROR_N(ident)
                            << "forward typedef '" << ident->get_name()
                            << "' is not defined at this reference (IEEE 1800-2017 6.18)";
                        return 1;
                    }
                    if(alias->unpacked_dims && !alias->unpacked_dims->empty()) {
                        LOG_ERROR_N(ident)
                            << "array typedef '" << ident->get_name() << "' is not legal here";
                        return 1;
                    }
                    parg->set_value(AST::cast_to<AST::DataType>(alias->type->clone()));
                }
            }
            return 0;
        }
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret |= substitute(child, node);
        }
        return ret;
    }

    // Subroutine bodies are nested scopes too (§6.18): the return type and
    // the ANSI args lexically precede the body, so they resolve against the
    // enclosing scope before the body scope opens.
    case AST::NodeType::Function: {
        const auto &function = AST::cast_to<AST::Function>(node);
        if(function->get_return_type() && substitute(function->get_return_type(), node)) {
            return 1;
        }
        if(function->get_args()) {
            for(const AST::Arg::Ptr &arg : *function->get_args()) {
                if(substitute(arg, node)) {
                    return 1;
                }
            }
        }
        m_scopes.emplace_back();
        const int ret = process_items(function->get_statements());
        m_scopes.pop_back();
        return ret;
    }

    case AST::NodeType::Task: {
        const auto &task = AST::cast_to<AST::Task>(node);
        if(task->get_args()) {
            for(const AST::Arg::Ptr &arg : *task->get_args()) {
                if(substitute(arg, node)) {
                    return 1;
                }
            }
        }
        m_scopes.emplace_back();
        const int ret = process_items(task->get_statements());
        m_scopes.pop_back();
        return ret;
    }

    // Nested scopes: their typedefs shadow enclosing bindings and are
    // registered/dropped in declaration order.
    case AST::NodeType::Block: {
        m_scopes.emplace_back();
        const int ret = process_items(AST::cast_to<AST::Block>(node)->get_statements());
        m_scopes.pop_back();
        return ret;
    }

    case AST::NodeType::ParallelBlock: {
        m_scopes.emplace_back();
        const int ret = process_items(AST::cast_to<AST::ParallelBlock>(node)->get_statements());
        m_scopes.pop_back();
        return ret;
    }

    case AST::NodeType::GenerateStatement: {
        m_scopes.emplace_back();
        const int ret = process_items(AST::cast_to<AST::GenerateStatement>(node)->get_items());
        m_scopes.pop_back();
        return ret;
    }

    default: {
        int ret = 0;
        const AST::Node::ListPtr children = node->get_children();
        for(const AST::Node::Ptr &child : *children) {
            ret |= substitute(child, node);
        }
        return ret;
    }
    }
}

int TypedefInliner::substitute_named_type(const AST::NamedType::Ptr &named,
                                          const AST::Node::Ptr &parent)
{
    const std::string &name = named->get_name();

    if(named->get_scope()) {
        // Package-scoped references are resolved (and their scope stripped)
        // by PackageInliner before this pass runs.
        LOG_ERROR_N(named) << "unresolved package-scoped type '" << name << "'";
        return 1;
    }

    const Alias *alias = lookup(name);
    if(!alias) {
        LOG_ERROR_N(named) << "'" << name << "' does not name a type";
        return 1;
    }
    if(!alias->complete) {
        LOG_ERROR_N(named) << "forward typedef '" << name
                           << "' is not defined at this reference (IEEE 1800-2017 6.18)";
        return 1;
    }

    const auto &cloned = AST::cast_to<AST::DataType>(alias->type->clone());

    // An array typedef (`typedef logic [7:0] mem_t [256]`) makes the alias an
    // array type: its dims move onto the use-site declaration, appended after
    // the declaration's own dims (the declaration's vary slowest, §6.18).
    const bool is_decl_type = parent && parent->is_node_category(AST::NodeType::Declaration) &&
                              AST::cast_to<AST::Declaration>(parent)->get_type() == named;
    if(alias->unpacked_dims && !alias->unpacked_dims->empty()) {
        const auto &decl = is_decl_type ? AST::cast_to<AST::Declaration>(parent) : nullptr;
        auto dims = Analysis::Dimensions::decl_unpacked_dims(decl);
        if(!dims) {
            dims = std::make_shared<AST::Dimension::List>();
        }
        const auto &extra = AST::Dimension::clone_list(alias->unpacked_dims);
        dims->insert(dims->end(), extra->begin(), extra->end());
        // The setter answers whether the declaration kind carries a slot at
        // all — the merged list is only built locally until it succeeds.
        if(!Analysis::Dimensions::set_decl_unpacked_dims(decl, dims)) {
            LOG_ERROR_N(named) << "array typedef '" << name << "' is not legal here";
            return 1;
        }
    }

    if(is_decl_type) {
        AST::cast_to<AST::Declaration>(parent)->set_type(cloned);
    } else if(parent) {
        parent->replace(named, cloned);
    } else {
        LOG_ERROR_N(named) << "type reference '" << name << "' has no enclosing node";
        return 1;
    }
    return 0;
}

int TypedefInliner::substitute_typedef_cast(const AST::TypeCast::Ptr &cast,
                                            const AST::Node::Ptr &parent)
{
    const auto &named = AST::cast_to<AST::NamedType>(cast->get_target());
    const std::string &name = named->get_name();

    if(named->get_scope()) {
        // Package-scoped references are resolved (and their scope stripped)
        // by PackageInliner before this pass runs. Looking up the bare name
        // could silently bind a same-named local typedef instead.
        LOG_ERROR_N(named) << "unresolved package-scoped type '" << name << "'";
        return 1;
    }

    const Alias *alias = lookup(name);
    if(!alias) {
        LOG_ERROR_N(named) << "'" << name << "' does not name a type";
        return 1;
    }
    if(!alias->complete) {
        LOG_ERROR_N(named) << "forward typedef '" << name
                           << "' is not defined at this reference (IEEE 1800-2017 6.18)";
        return 1;
    }
    if(alias->unpacked_dims && !alias->unpacked_dims->empty()) {
        LOG_ERROR_N(named) << "array typedef '" << name << "' is not legal here";
        return 1;
    }

    // An integral alias lowers to a size cast of its packed width (§6.24.1):
    // a vector type contributes its dims, an integer atom its fixed width
    // (§6.11). A size cast keeps the operand's signedness, so a signed alias
    // additionally wraps in a signing cast. Reject non-integral aliases
    // loudly rather than mis-render.
    const auto &type = alias->type;
    bool is_signed = false;
    std::size_t width = 1;
    switch(type->get_node_type()) {
    case AST::NodeType::LogicType:
    case AST::NodeType::RegType:
    case AST::NodeType::BitType:
    case AST::NodeType::ImplicitType:
        // Vector types default to unsigned (§6.11).
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        break;
    case AST::NodeType::ByteType:
        width = 8;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::ShortintType:
        width = 16;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::IntType:
    case AST::NodeType::IntegerType:
        width = 32;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::LongintType:
        width = 64;
        is_signed = type->get_signing() != AST::DataType::SigningEnum::UNSIGNED;
        break;
    case AST::NodeType::TimeType:
        // `time` is the one unsigned integer atom (§6.11).
        width = 64;
        is_signed = type->get_signing() == AST::DataType::SigningEnum::SIGNED;
        break;
    default:
        LOG_ERROR_N(named) << "cast to typedef '" << name
                           << "': only an integral alias is supported";
        return 1;
    }

    if(type->get_packed_dims()) {
        for(const AST::Dimension::Ptr &dim : *type->get_packed_dims()) {
            Analysis::Dimensions::DimInfo info;
            if(!Analysis::Dimensions::extract_dimension(dim, Analysis::Dimensions::Packing::packed,
                                                        info)) {
                LOG_ERROR_N(named)
                    << "cast to typedef '" << name << "': the alias width is not constant";
                return 1;
            }
            width *= info.width;
        }
    }

    const auto &size = std::make_shared<AST::IntConstN>(10, -1, true, Misc::Math::u64_to_mpz(width),
                                                        cast->get_filename(), cast->get_line());
    AST::Node::Ptr lowered = std::make_shared<AST::SizeCast>(
        size, cast->get_expr(), cast->get_filename(), cast->get_line());
    if(is_signed) {
        lowered = std::make_shared<AST::SigningCast>(lowered, AST::SigningCast::SigningEnum::SIGNED,
                                                     cast->get_filename(), cast->get_line());
    }
    if(!parent) {
        LOG_ERROR_N(cast) << "cast to typedef '" << name << "' has no enclosing node";
        return 1;
    }
    parent->replace(cast, lowered);
    return 0;
}

const TypedefInliner::Alias *TypedefInliner::lookup(const std::string &name) const
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
