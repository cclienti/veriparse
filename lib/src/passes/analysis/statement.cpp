// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/statement.hpp>
#include <veriparse/AST/node_cast.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

void Statement::collect_identifier_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::Identifier)) {
        names.insert(AST::cast_to<AST::Identifier>(node)->get_name());
        return;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        collect_identifier_names(child, names);
    }
}

std::string Statement::lvalue_target(const AST::Lvalue::Ptr &lvalue)
{
    if(!lvalue || !lvalue->get_var()) {
        return "";
    }
    if(!lvalue->get_var()->is_node_type(AST::NodeType::Identifier)) {
        return "";
    }
    return AST::cast_to<AST::Identifier>(lvalue->get_var())->get_name();
}

std::string Statement::nba_target(const AST::NonblockingSubstitution::Ptr &nba)
{
    return lvalue_target(nba->get_left());
}

void Statement::collect_call_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    if(node->is_node_type(AST::NodeType::TaskCall) || node->is_node_type(AST::NodeType::Call)) {
        names.insert(AST::cast_to<AST::Call>(node)->get_name());
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        collect_call_names(child, names);
    }
}

void Statement::collect_lvalue_bases(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Identifier:
        // A hierarchical write (u.q, genblk.q) targets another scope, not
        // this module's register of the same leaf name.
        if(AST::cast_to<AST::Identifier>(node)->get_hier()) {
            return;
        }
        names.insert(AST::cast_to<AST::Identifier>(node)->get_name());
        return;
    case AST::NodeType::Lvalue:
        collect_lvalue_bases(AST::cast_to<AST::Lvalue>(node)->get_var(), names);
        return;
    case AST::NodeType::Pointer:
        collect_lvalue_bases(AST::cast_to<AST::Pointer>(node)->get_var(), names);
        return;
    case AST::NodeType::Partselect:
        collect_lvalue_bases(AST::cast_to<AST::Partselect>(node)->get_var(), names);
        return;
    case AST::NodeType::PartselectIndexed:
    case AST::NodeType::PartselectPlusIndexed:
    case AST::NodeType::PartselectMinusIndexed:
        // The base is driven; the index expression is a read.
        collect_lvalue_bases(AST::cast_to<AST::PartselectIndexed>(node)->get_var(), names);
        return;
    default: {
        // Concatenations and anything else: every child may name a target.
        const auto &children = node->get_children();
        for(const auto &child : *children) {
            collect_lvalue_bases(child, names);
        }
        return;
    }
    }
}

void Statement::collect_driven(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::NonblockingSubstitution:
    case AST::NodeType::BlockingSubstitution:
    case AST::NodeType::Assign:
        collect_lvalue_bases(AST::to_node(AST::cast_to<AST::Assign>(node)->get_left()), names);
        return;
    default: {
        const auto &children = node->get_children();
        for(const auto &child : *children) {
            collect_driven(child, names);
        }
        return;
    }
    }
}

void Statement::collect_declaration_names(const AST::Node::Ptr &node, std::set<std::string> &names)
{
    if(!node) {
        return;
    }
    const auto &decl = std::dynamic_pointer_cast<AST::Declaration>(node);
    if(decl) {
        names.insert(decl->get_name());
        return;
    }
    const auto &children = node->get_children();
    for(const auto &child : *children) {
        collect_declaration_names(child, names);
    }
}

void Statement::collect_reads(const AST::Node::Ptr &node, std::set<std::string> &reads)
{
    if(!node) {
        return;
    }
    switch(node->get_node_type()) {
    case AST::NodeType::Block: {
        const auto &stmts = AST::cast_to<AST::Block>(node)->get_statements();
        if(stmts) {
            for(const auto &stmt : *stmts) {
                collect_reads(stmt, reads);
            }
        }
        break;
    }
    case AST::NodeType::NonblockingSubstitution:
        collect_identifier_names(
            AST::to_node(AST::cast_to<AST::NonblockingSubstitution>(node)->get_right()), reads);
        break;
    case AST::NodeType::IfStatement: {
        const auto &ifs = AST::cast_to<AST::IfStatement>(node);
        collect_identifier_names(ifs->get_cond(), reads);
        collect_reads(ifs->get_true_statement(), reads);
        collect_reads(ifs->get_false_statement(), reads);
        break;
    }
    case AST::NodeType::CaseStatement:
    case AST::NodeType::CasexStatement:
    case AST::NodeType::CasezStatement: {
        const auto &cs = AST::cast_to<AST::CaseStatement>(node);
        collect_identifier_names(cs->get_comp(), reads);
        const auto &caselist = cs->get_caselist();
        if(caselist) {
            for(const auto &arm : *caselist) {
                const auto &conds = arm->get_cond();
                if(conds) {
                    for(const auto &value : *conds) {
                        collect_identifier_names(value, reads);
                    }
                }
                collect_reads(arm->get_statement(), reads);
            }
        }
        break;
    }
    default:
        break;
    }
}

int Statement::first_reference(const AST::Node::Ptr &node, const std::string &name)
{
    if(!node) {
        return 0;
    }
    if(node->is_node_type(AST::NodeType::NonblockingSubstitution) ||
       node->is_node_type(AST::NodeType::BlockingSubstitution)) {
        const auto &parts = node->get_children();
        if(!parts || parts->empty()) {
            return 0;
        }
        const AST::Node::Ptr left = parts->front();
        std::set<std::string> targets;
        collect_driven(node, targets);
        if(targets.count(name)) {
            // Program order within one statement is RHS-then-commit, so
            // a self-read on the right counts as a read.
            std::set<std::string> rhs_reads;
            for(const auto &child : *parts) {
                if(child != left) {
                    collect_identifier_names(child, rhs_reads);
                }
            }
            return rhs_reads.count(name) ? -1 : 1;
        }
        std::set<std::string> reads;
        collect_identifier_names(node, reads);
        return reads.count(name) ? -1 : 0;
    }
    if(node->is_node_category(AST::NodeType::Identifier) &&
       AST::cast_to<AST::Identifier>(node)->get_name() == name) {
        // Outside an assignment — a condition, an event control, a bound,
        // an actual — an occurrence can only read.
        return -1;
    }
    const auto &children = node->get_children();
    if(children) {
        for(const auto &child : *children) {
            const int verdict = first_reference(child, name);
            if(verdict) {
                return verdict;
            }
        }
    }
    return 0;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
