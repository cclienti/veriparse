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

AnnotateDeclaration::AnnotateDeclaration():
    m_ignore_io (true),
    m_search    ("^.*$"),
    m_replace   ("$&")
{}

AnnotateDeclaration::AnnotateDeclaration(const std::string &search, const std::string &replace, bool ignore_io):
    m_ignore_io (ignore_io),
    m_search    (search),
    m_replace   (replace)
{}

void AnnotateDeclaration::set_search_replace(const std::string &search,
                                            const std::string &replace)
{
    m_search = search;
    m_replace = replace;
}

int AnnotateDeclaration::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    ReplaceDict replace_dict;
    std::set<std::string> excluded_names;

    if (m_ignore_io) {
        std::vector<std::string> iodir_names = Analysis::Module::get_iodir_names(node);
        excluded_names.insert(iodir_names.cbegin(), iodir_names.cend());
    }

    if(get_declaration_names(node, replace_dict, excluded_names))
        return 1;

    if(annotate_names(node, replace_dict))
        return 1;

    return 0;
}

int AnnotateDeclaration::get_declaration_names(const AST::Node::Ptr node, ReplaceDict &replace_dict,
                                               const std::set<std::string> &excluded_names)
{
    int rc = 0;

    if (node) {
        if (node->is_node_category(AST::NodeType::VariableBase)) {
            AST::VariableBase::Ptr varbase = AST::cast_to<AST::VariableBase>(node);
            std::string name = varbase->get_name();

            if (excluded_names.count(name) == 0) {
                if (replace_dict.count(name) != 0) {
                    LOG_ERROR_N(node) << "redefinition of '" << name << "'";
                    return 1;
                }
                replace_dict[name] = std::regex_replace(name, m_search, m_replace);
            }
        }
        else if (node->is_node_type(AST::NodeType::Task)) {
            AST::Task::Ptr task = AST::cast_to<AST::Task>(node);
            std::string name = task->get_name();

            if (excluded_names.count(name) == 0) {
                if (replace_dict.count(name) != 0) {
                    LOG_ERROR_N(node) << "redefinition of '" << name << "'";
                    return 1;
                }
                replace_dict[name] = std::regex_replace(name, m_search, m_replace);
            }
        }
        else if (node->is_node_type(AST::NodeType::Function)) {
            AST::Function::Ptr function = AST::cast_to<AST::Function>(node);
            std::string name = function->get_name();

            if (excluded_names.count(name) == 0) {
                if (replace_dict.count(name) != 0) {
                    LOG_ERROR_N(node) << "redefinition of '" << name << "'";
                    return 1;
                }
                replace_dict[name] = std::regex_replace(name, m_search, m_replace);
            }
        }
        else if (node->is_node_type(AST::NodeType::Localparam)) {
            AST::Localparam::Ptr localparam = AST::cast_to<AST::Localparam>(node);
            std::string name = localparam->get_name();

            if (excluded_names.count(name) == 0) {
                if (replace_dict.count(name) != 0) {
                    LOG_ERROR_N(node) << "redefinition of '" << name << "'";
                    return 1;
                }
                replace_dict[name] = std::regex_replace(name, m_search, m_replace);
            }
        }
        else {
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= get_declaration_names(child, replace_dict, excluded_names);
            }
        }
    }
    else {
        LOG_ERROR << "Empty node found";
        return 1;
    }

    return rc;
}

int AnnotateDeclaration::annotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict)
{
    int rc = 0;

    if (node) {
        if(node->is_node_category(AST::NodeType::VariableBase)) {
            AST::VariableBase::Ptr variable = AST::cast_to<AST::VariableBase>(node);
            const std::string &name = variable->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                variable->set_name(new_name);
            }
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, replace_dict);
            }
        }

        else if (node->is_node_type(AST::NodeType::Identifier)) {
            AST::Identifier::Ptr identifier = AST::cast_to<AST::Identifier>(node);
            const std::string &name = identifier->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                identifier->set_name(new_name);
            }
        }

        else if (node->is_node_type(AST::NodeType::Localparam)) {
            AST::Localparam::Ptr localparam = AST::cast_to<AST::Localparam>(node);
            const std::string &name = localparam->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                localparam->set_name(new_name);
            }
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, replace_dict);
            }
        }

        else if (node->is_node_type(AST::NodeType::Function)) {
            AST::Function::Ptr function = AST::cast_to<AST::Function>(node);
            const std::string &name = function->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                function->set_name(new_name);
            }
            std::set<std::string> locals = merge_set(to_set(Analysis::Function::get_iodir_names(node)),
                                                     to_set(Analysis::Function::get_variable_names(node)));
            ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, new_replace_dict);
            }
        }

        else if (node->is_node_type(AST::NodeType::FunctionCall)) {
            AST::FunctionCall::Ptr fcall = AST::cast_to<AST::FunctionCall>(node);
            const std::string &name = fcall->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                fcall->set_name(new_name);
            }
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, replace_dict);
            }
        }

        else if (node->is_node_type(AST::NodeType::Task)) {
            AST::Task::Ptr task = AST::cast_to<AST::Task>(node);
            const std::string &name = task->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                task->set_name(new_name);
            }
            std::set<std::string> locals = merge_set(to_set(Analysis::Task::get_iodir_names(node)),
                                                     to_set(Analysis::Task::get_variable_names(node)));
            ReplaceDict new_replace_dict = remove_keys(replace_dict, locals);
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, new_replace_dict);
            }
        }

        else if (node->is_node_type(AST::NodeType::TaskCall)) {
            AST::TaskCall::Ptr tcall = AST::cast_to<AST::TaskCall>(node);
            const std::string &name = tcall->get_name();
            if(replace_dict.count(name)) {
                const std::string &new_name = replace_dict[name];
                tcall->set_name(new_name);
            }
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, replace_dict);
            }
        }

        else {
            AST::Node::ListPtr children = node->get_children();
            for (AST::Node::Ptr child: *children) {
                rc |= annotate_names(child, replace_dict);
            }
        }
    }

    else {
        LOG_ERROR << "Empty node found";
        return 1;
    }

    return rc;
}

}
}
}
