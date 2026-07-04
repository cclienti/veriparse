// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "./transformation_helpers.hpp"

#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/function.hpp>
#include <veriparse/passes/analysis/task.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <iterator>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

AnnotateDeclaration::AnnotateDeclaration() : m_ignore_io(true), m_search("^.*$"), m_replace("$&") {}

AnnotateDeclaration::AnnotateDeclaration(const std::string &search, const std::string &replace,
                                         bool ignore_io)
    : m_ignore_io(ignore_io), m_search(search), m_replace(replace)
{
}

void AnnotateDeclaration::set_search_replace(const std::string &search, const std::string &replace)
{
    m_search = search;
    m_replace = replace;
}

int AnnotateDeclaration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    ReplaceDict replace_dict;
    std::set<std::string> excluded_names;

    if(m_ignore_io) {
        std::vector<std::string> iodir_names = Analysis::Module::get_iodir_names(node);
        excluded_names.insert(iodir_names.cbegin(), iodir_names.cend());
    }

    if(get_declaration_names(node, replace_dict, excluded_names)) {
        return 1;
    }

    if(annotate_names(node, replace_dict)) {
        return 1;
    }

    return 0;
}

int AnnotateDeclaration::get_declaration_names(const AST::Node::Ptr node, ReplaceDict &replace_dict,
                                               const std::set<std::string> &excluded_names)
{
    int rc = 0;

    if(!node) {
        LOG_ERROR << "Empty node found";
        return 1;
    }

    bool found_name = true;
    std::string name;

    if(node->is_node_type(AST::NodeType::Var) || node->is_node_category(AST::NodeType::Net)) {
        name = AST::cast_to<AST::Declaration>(node)->get_name();
    } else if(node->is_node_type(AST::NodeType::Genvar)) {
        name = AST::cast_to<AST::Genvar>(node)->get_name();
    } else if(node->is_node_category(AST::NodeType::Instance)) {
        const auto &instance = AST::cast_to<AST::Instance>(node);
        name = instance->get_name();
    } else if(node->is_node_type(AST::NodeType::Task)) {
        const auto &task = AST::cast_to<AST::Task>(node);
        name = task->get_name();
    } else if(node->is_node_type(AST::NodeType::Function)) {
        const auto &function = AST::cast_to<AST::Function>(node);
        name = function->get_name();
    } else if(node->is_node_type(AST::NodeType::Param) &&
              AST::cast_to<AST::Param>(node)->get_is_local()) {
        name = AST::cast_to<AST::Param>(node)->get_name();
    } else {
        found_name = false;
        const auto &children = node->get_children();
        for(const auto &child : *children) {
            rc |= get_declaration_names(child, replace_dict, excluded_names);
        }
    }

    if(found_name) {
        if(excluded_names.count(name) == 0) {
            replace_dict[name] = std::regex_replace(name, m_search, m_replace);
        }
    }

    return rc;
}

int AnnotateDeclaration::annotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict)
{
    int rc = 0;

    if(!node) {
        LOG_ERROR << "Empty node found";
        return 1;
    }

    if(node->is_node_type(AST::NodeType::Var) || node->is_node_category(AST::NodeType::Net)) {
        const auto &variable = AST::cast_to<AST::Declaration>(node);
        const std::string &name = variable->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            variable->set_name(new_name);
        }
    }

    else if(node->is_node_type(AST::NodeType::Genvar)) {
        const auto &genvar = AST::cast_to<AST::Genvar>(node);
        const std::string &name = genvar->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            genvar->set_name(new_name);
        }
    }

    else if(node->is_node_type(AST::NodeType::Identifier)) {
        const auto &identifier = AST::cast_to<AST::Identifier>(node);
        const std::string &name = identifier->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            identifier->set_name(new_name);
        }
    }

    else if(node->is_node_type(AST::NodeType::Param) &&
            AST::cast_to<AST::Param>(node)->get_is_local()) {
        const auto &localparam = AST::cast_to<AST::Param>(node);
        const std::string &name = localparam->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            localparam->set_name(new_name);
        }
    }

    else if(node->is_node_category(AST::NodeType::Instance)) {
        const auto &instance = AST::cast_to<AST::Instance>(node);
        const std::string &name = instance->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            instance->set_name(new_name);
        }
    }

    else if(node->is_node_type(AST::NodeType::Function)) {
        const auto &function = AST::cast_to<AST::Function>(node);
        const std::string &name = function->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            function->set_name(new_name);
        }
        std::set<std::string> locals =
            merge_set(to_set(Analysis::Function::get_iodir_names(node)),
                      to_set(Analysis::Function::get_variable_names(node)));
        ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
        const auto &children = node->get_children();
        for(const auto &child : *children) {
            rc |= annotate_names(child, new_replace_dict);
        }
        return rc;
    }

    // A call to a renamed function/task: the neutral Call, FunctionCall and
    // TaskCall all derive from Call and carry the callee name.
    else if(node->is_node_category(AST::NodeType::Call)) {
        const auto &call = AST::cast_to<AST::Call>(node);
        const std::string &name = call->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            call->set_name(new_name);
        }
    }

    else if(node->is_node_type(AST::NodeType::Task)) {
        const auto &task = AST::cast_to<AST::Task>(node);
        const std::string &name = task->get_name();
        if(replace_dict.count(name)) {
            const std::string &new_name = replace_dict[name];
            task->set_name(new_name);
        }
        std::set<std::string> locals = merge_set(to_set(Analysis::Task::get_iodir_names(node)),
                                                 to_set(Analysis::Task::get_variable_names(node)));
        ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
        const auto &children = node->get_children();
        for(const auto &child : *children) {
            rc |= annotate_names(child, new_replace_dict);
        }
        return rc;
    }

    const auto &children = node->get_children();
    for(const auto &child : *children) {
        rc |= annotate_names(child, replace_dict);
    }

    return rc;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
