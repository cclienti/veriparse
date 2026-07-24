// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/type_param_inliner.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

namespace
{

bool has_type_param(const AST::Declaration::ListPtr &params)
{
    if(!params) {
        return false;
    }
    for(const AST::Declaration::Ptr &decl : *params) {
        if(decl->is_node_type(AST::NodeType::TypeParam)) {
            return true;
        }
    }
    return false;
}

} // namespace

TypeParamInliner::TypeParamInliner() : m_paramlist_inst(nullptr) {}

TypeParamInliner::TypeParamInliner(AST::ParamArg::ListPtr paramlist_inst)
    : m_paramlist_inst(paramlist_inst)
{
}

int TypeParamInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Module: {
        const auto &module = AST::cast_to<AST::Module>(node);
        if(!module->get_items() && has_type_param(module->get_params())) {
            // A bodiless module still receives its reduced typedefs.
            module->set_items(std::make_shared<AST::Node::List>());
        }
        return process_scope(
            module->get_params(),
            [&module](const AST::Declaration::ListPtr &p) { module->set_params(p); },
            module->get_items(), node);
    }

    case AST::NodeType::Interface: {
        const auto &interface = AST::cast_to<AST::Interface>(node);
        if(!interface->get_items() && has_type_param(interface->get_params())) {
            interface->set_items(std::make_shared<AST::Node::List>());
        }
        return process_scope(
            interface->get_params(),
            [&interface](const AST::Declaration::ListPtr &p) { interface->set_params(p); },
            interface->get_items(), node);
    }

    case AST::NodeType::TypeParam: {
        // A body declaration reduces in place, keeping its §6.18 position.
        const auto &tdef = reduce(AST::cast_to<AST::TypeParam>(node));
        if(!tdef) {
            return 1;
        }
        if(!parent) {
            LOG_ERROR_N(node) << "type parameter '"
                              << AST::cast_to<AST::TypeParam>(node)->get_name()
                              << "' has no enclosing node";
            return 1;
        }
        parent->replace(node, tdef);
        return 0;
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

int TypeParamInliner::process_scope(
    const AST::Declaration::ListPtr &params,
    const std::function<void(const AST::Declaration::ListPtr &)> &set_params,
    const AST::Node::ListPtr &items, const AST::Node::Ptr &node)
{
    // Header formals: bind and pull out, in declaration order.
    std::list<AST::Typedef::Ptr> reduced;
    if(params) {
        for(auto it = params->begin(); it != params->end();) {
            if((*it)->is_node_type(AST::NodeType::TypeParam)) {
                const auto &tdef = reduce(AST::cast_to<AST::TypeParam>(*it));
                if(!tdef) {
                    return 1;
                }
                reduced.push_back(tdef);
                it = params->erase(it);
            } else {
                ++it;
            }
        }
        if(params->empty()) {
            set_params(nullptr);
        }
    }

    // Splice after the leading typedef run at the body head: a formal's
    // default may reference a package/unit typedef PackageInliner spliced
    // there, and TypedefInliner registers in declaration order (§6.18).
    // Header ports and parameters see the result either way (ADR-0009 §2).
    if(!reduced.empty()) {
        if(!items) {
            LOG_ERROR_N(node) << "type parameter '" << reduced.front()->get_name()
                              << "' in a bodiless scope";
            return 1;
        }
        auto pos = items->begin();
        while(pos != items->end() && (*pos)->is_node_type(AST::NodeType::Typedef)) {
            ++pos;
        }
        items->insert(pos, reduced.begin(), reduced.end());
    }

    // Body declarations (including nested generate scopes) reduce in place.
    int ret = 0;
    if(items) {
        for(const AST::Node::Ptr &item : *items) {
            ret |= process(item, node);
        }
    }
    return ret;
}

AST::Typedef::Ptr TypeParamInliner::reduce(const AST::TypeParam::Ptr &tparam) const
{
    const std::string &name = tparam->get_name();

    AST::DataType::Ptr bound = tparam->get_type();
    AST::Dimension::ListPtr bound_dims;
    bool overridden = false;
    if(m_paramlist_inst) {
        for(const AST::ParamArg::Ptr &arg : *m_paramlist_inst) {
            if(!arg || arg->get_name() != name) {
                continue;
            }
            const auto &value = arg->get_value();
            if(!value) {
                // A synthesized use-the-default marker (instance normalizer).
                break;
            }
            if(tparam->get_is_local()) {
                LOG_ERROR_N(tparam)
                    << "'" << name << "' is a localparam type and cannot be overridden";
                return nullptr;
            }
            if(value->is_node_type(AST::NodeType::Typedef)) {
                // An array-typedef actual: the parent packaged the alias's
                // type and unpacked dims together (ADR-0010 §7, ADR-0009 §5).
                bound = AST::cast_to<AST::Typedef>(value)->get_type();
                bound_dims = AST::cast_to<AST::Typedef>(value)->get_unpacked_dims();
            } else if(value->is_node_category(AST::NodeType::DataType)) {
                bound = AST::cast_to<AST::DataType>(value);
            } else {
                // The parent resolves identifier actuals to concrete types
                // (ADR-0010 §4); an expression surviving to here is not a
                // type.
                LOG_ERROR_N(tparam) << "override of type parameter '" << name << "' is not a type";
                return nullptr;
            }
            overridden = true;
            break;
        }
    }

    if(!bound) {
        LOG_ERROR_N(tparam) << "type parameter '" << name
                            << "' has no default and no override (IEEE 1800-2017 6.20.3)";
        return nullptr;
    }

    // An override's packed dims were folded in the instantiating scope
    // (ADR-0010 §5): a non-constant width there is a runtime-dependent type.
    // A default's dims may still reference sibling value formals — they fold
    // later in the pipeline, so only overrides are checked here.
    if(overridden && bound->get_packed_dims()) {
        for(const AST::Dimension::Ptr &dim : *bound->get_packed_dims()) {
            Analysis::Dimensions::DimInfo info;
            if(!Analysis::Dimensions::extract_dimension(dim, Analysis::Dimensions::Packing::packed,
                                                        info)) {
                LOG_ERROR_N(tparam)
                    << "type parameter '" << name << "': the bound type's width is not constant "
                    << "(IEEE 1800-2017 6.20.2)";
                return nullptr;
            }
        }
    }

    const auto &tdef = std::make_shared<AST::Typedef>(tparam->get_filename(), tparam->get_line());
    tdef->set_name(name);
    tdef->set_type(AST::cast_to<AST::DataType>(bound->clone()));
    if(bound_dims) {
        tdef->set_unpacked_dims(AST::Dimension::clone_list(bound_dims));
    }
    return tdef;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
