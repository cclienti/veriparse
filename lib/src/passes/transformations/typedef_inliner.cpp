// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/typedef_inliner.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

/// The unpacked-dims list of a declaration, or nullptr when the declaration
/// kind carries none. The accessor is per-subclass (no common virtual), hence
/// the dispatch.
AST::Dimension::ListPtr decl_unpacked_dims(const AST::Declaration::Ptr &decl)
{
    if(decl->is_node_type(AST::NodeType::Var)) {
        return AST::cast_to<AST::Var>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_type(AST::NodeType::Arg)) {
        return AST::cast_to<AST::Arg>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_type(AST::NodeType::Member)) {
        return AST::cast_to<AST::Member>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_type(AST::NodeType::Param)) {
        return AST::cast_to<AST::Param>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_type(AST::NodeType::Typedef)) {
        return AST::cast_to<AST::Typedef>(decl)->get_unpacked_dims();
    }
    if(decl->is_node_category(AST::NodeType::Net)) {
        return AST::cast_to<AST::Net>(decl)->get_unpacked_dims();
    }
    return nullptr;
}

/// True when the declaration kind carries an unpacked-dims slot.
bool decl_has_unpacked_dims_slot(const AST::Declaration::Ptr &decl)
{
    return decl->is_node_type(AST::NodeType::Var) || decl->is_node_type(AST::NodeType::Arg) ||
           decl->is_node_type(AST::NodeType::Member) || decl->is_node_type(AST::NodeType::Param) ||
           decl->is_node_type(AST::NodeType::Typedef) || decl->is_node_category(AST::NodeType::Net);
}

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

void set_decl_unpacked_dims(const AST::Declaration::Ptr &decl, const AST::Dimension::ListPtr &dims)
{
    if(decl->is_node_type(AST::NodeType::Var)) {
        AST::cast_to<AST::Var>(decl)->set_unpacked_dims(dims);
    } else if(decl->is_node_type(AST::NodeType::Arg)) {
        AST::cast_to<AST::Arg>(decl)->set_unpacked_dims(dims);
    } else if(decl->is_node_type(AST::NodeType::Member)) {
        AST::cast_to<AST::Member>(decl)->set_unpacked_dims(dims);
    } else if(decl->is_node_type(AST::NodeType::Param)) {
        AST::cast_to<AST::Param>(decl)->set_unpacked_dims(dims);
    } else if(decl->is_node_type(AST::NodeType::Typedef)) {
        AST::cast_to<AST::Typedef>(decl)->set_unpacked_dims(dims);
    } else if(decl->is_node_category(AST::NodeType::Net)) {
        AST::cast_to<AST::Net>(decl)->set_unpacked_dims(dims);
    }
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
        if(!decl || !decl_has_unpacked_dims_slot(decl)) {
            LOG_ERROR_N(named) << "array typedef '" << name << "' is not legal here";
            return 1;
        }
        auto dims = decl_unpacked_dims(decl);
        if(!dims) {
            dims = std::make_shared<AST::Dimension::List>();
        }
        const auto &extra = AST::Dimension::clone_list(alias->unpacked_dims);
        dims->insert(dims->end(), extra->begin(), extra->end());
        set_decl_unpacked_dims(decl, dims);
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
