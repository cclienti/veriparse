// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/fsm_alpha_rename.hpp>
#include "implicit_fsm_detail.hpp"
#include <veriparse/passes/transformations/scope_table.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

using namespace FsmDetail;

int FsmAlphaRename::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }
    if(!node->is_node_type(AST::NodeType::Module)) {
        return recurse_in_childs(node);
    }
    return rename_module(AST::cast_to<AST::Module>(node));
}

int FsmAlphaRename::rename_module(const AST::Module::Ptr &module)
{
    std::vector<AST::Initial::Ptr> initials;
    for(const auto &pair : collect_marked(module)) {
        if(pair.second->is_node_type(AST::NodeType::Initial)) {
            initials.push_back(AST::cast_to<AST::Initial>(pair.second));
        }
    }
    if(initials.empty()) {
        return 0;
    }

    m_declared.clear();
    m_used.clear();
    if(Analysis::UniqueDeclaration::analyze(AST::to_node(module), m_declared)) {
        LOG_ERROR_N(module) << "failed to analyze declarations";
        return 1;
    }
    // The module's own scope, shallow: what a process-level temporary can
    // shadow. The deep set above serves freshness only — it holds the
    // block locals themselves, which must not conflict with themselves.
    m_module_names.clear();
    {
        const auto &ports = module->get_ports();
        if(ports) {
            for(const auto &port : *ports) {
                if(port->is_node_type(AST::NodeType::Port)) {
                    m_module_names.insert(AST::cast_to<AST::Port>(port)->get_name());
                }
            }
        }
        const auto &items = module->get_items();
        if(items) {
            for(const auto &item : *items) {
                ScopeTable::for_each_bound_name(
                    item, [this](const std::string &name) { m_module_names.insert(name); });
            }
        }
    }

    // The tasks the marked processes reach, transitively: their locals
    // take part in the same substitutions once inlined.
    std::map<std::string, AST::Task::Ptr> tasks;
    {
        const auto &task_nodes = Analysis::Module::get_task_nodes(AST::to_node(module));
        if(task_nodes) {
            for(const auto &task : *task_nodes) {
                tasks[task->get_name()] = task;
            }
        }
    }
    std::set<std::string> reached;
    {
        std::vector<std::string> worklist;
        for(const auto &initial : initials) {
            std::set<std::string> called;
            collect_call_names(AST::to_node(initial), called);
            worklist.insert(worklist.end(), called.begin(), called.end());
        }
        while(!worklist.empty()) {
            const std::string name = worklist.back();
            worklist.pop_back();
            if(!tasks.count(name) || !reached.insert(name).second) {
                continue;
            }
            std::set<std::string> called;
            collect_call_names(AST::to_node(tasks.at(name)), called);
            worklist.insert(worklist.end(), called.begin(), called.end());
        }
    }
    for(const auto &name : reached) {
        if(rename_task(tasks.at(name))) {
            return 1;
        }
    }

    for(const auto &initial : initials) {
        m_module_wide = true;
        m_seen.clear();
        std::vector<Frame> frames;
        if(rename_refs(initial->get_statement(), frames)) {
            return 1;
        }
    }
    return 0;
}

int FsmAlphaRename::rename_task(const AST::Task::Ptr &task)
{
    // The formals are the task's outermost bindings; the locals uniquify
    // against them and each other, never against the module — the
    // inliner's site prefixes keep task storage clear of module names.
    m_module_wide = false;
    m_seen.clear();
    std::vector<Frame> frames;
    frames.emplace_back();
    const auto &formals = task->get_args();
    if(formals) {
        for(const auto &formal : *formals) {
            frames.back()[formal->get_name()] = formal->get_name();
            m_seen.insert(formal->get_name());
        }
    }
    const auto &stmts = task->get_statements();
    if(stmts) {
        for(const auto &stmt : *stmts) {
            if(rename_refs(stmt, frames)) {
                return 1;
            }
        }
    }
    return 0;
}

bool FsmAlphaRename::bound_in_frames(const std::string &name,
                                     const std::vector<Frame> &frames) const
{
    for(const auto &frame : frames) {
        if(frame.count(name)) {
            return true;
        }
    }
    return false;
}

std::string FsmAlphaRename::fresh_name(const std::string &base)
{
    for(unsigned int ordinal = 0;; ++ordinal) {
        const std::string candidate = base + "_" + std::to_string(ordinal);
        if(!Analysis::UniqueDeclaration::identifier_declaration_exists(candidate, m_declared) &&
           !m_used.count(candidate)) {
            m_used.insert(candidate);
            return candidate;
        }
    }
}

int FsmAlphaRename::rename_list(const AST::Node::ListPtr &stmts, std::vector<Frame> &frames)
{
    frames.emplace_back();
    if(stmts) {
        for(const auto &stmt : *stmts) {
            if(!stmt) {
                continue;
            }
            if(stmt->is_node_type(AST::NodeType::Var)) {
                const auto &var = AST::cast_to<AST::Var>(stmt);
                const std::string name = var->get_name();
                const bool conflict =
                    m_seen.count(name) != 0 || (m_module_wide && m_module_names.count(name));
                m_seen.insert(name);
                if(conflict) {
                    const std::string renamed = fresh_name(name);
                    var->set_name(renamed);
                    frames.back()[name] = renamed;
                    m_seen.insert(renamed);
                } else {
                    frames.back()[name] = name;
                }
                // The initializer resolves in the declaration's own scope.
                if(var->get_init() && rename_refs(AST::to_node(var->get_init()), frames)) {
                    frames.pop_back();
                    return 1;
                }
                continue;
            }
            if(rename_refs(stmt, frames)) {
                frames.pop_back();
                return 1;
            }
        }
    }
    frames.pop_back();
    return 0;
}

int FsmAlphaRename::rename_refs(const AST::Node::Ptr &node, std::vector<Frame> &frames)
{
    if(!node) {
        return 0;
    }
    // Only the exact Identifier renames: a Call subtype's name is a
    // subroutine, never a block local; a hierarchical leaf names another
    // scope.
    if(node->get_node_type() == AST::NodeType::Identifier) {
        const auto &id = AST::cast_to<AST::Identifier>(node);
        if(!id->get_hier()) {
            for(auto it = frames.rbegin(); it != frames.rend(); ++it) {
                const auto &found = it->find(id->get_name());
                if(found != it->end()) {
                    if(found->second != id->get_name()) {
                        id->set_name(found->second);
                    }
                    break;
                }
            }
        }
        return 0;
    }
    if(node->is_node_type(AST::NodeType::Block)) {
        return rename_list(AST::cast_to<AST::Block>(node)->get_statements(), frames);
    }
    const auto &children = node->get_children();
    if(!children) {
        return 0;
    }
    for(const auto &child : *children) {
        if(rename_refs(child, frames)) {
            return 1;
        }
    }
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
