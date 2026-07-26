// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_DECLARATION_HELPERS
#define VERIPARSE_PASSES_ANALYSIS_DECLARATION_HELPERS

#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>

#include <utility>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

/**
 * @brief A declared signal is a Var or any Net — but NOT a bare port
 * placeholder. A non-ANSI directional declaration with neither a net nor
 * a data type keyword (`input clock`, `input [7:0] x`) parses as an
 * ImplicitNet carrying an ImplicitType; its real signal is declared
 * separately (`wire clock`) or defaults to a net, so it is not itself a
 * variable declaration. An ImplicitNet with a concrete data type
 * (`output reg valid` -> ImplicitNet + RegType) IS a real declaration
 * and is kept. After DefaultResolution (ADR-0012) the placeholder shape
 * survives only for a direction declaration backed by a standalone body
 * declaration — for which this verdict stays exact.
 */
inline bool is_declared_signal(const AST::Declaration::Ptr &d)
{
    if(!(d->is_node_type(AST::NodeType::Var) || d->is_node_category(AST::NodeType::Net))) {
        return false;
    }
    if(d->is_node_type(AST::NodeType::ImplicitNet)) {
        const AST::DataType::Ptr type = d->get_type();
        if(!type || type->is_node_type(AST::NodeType::ImplicitType)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Visit every standalone body Var/Net declaration reachable from
 * @p node: declaration statements at any block/generate depth, but never
 * inside a Port wrapper (that is the direction declaration itself), a
 * subroutine, or a process. @p visit receives the declaration and its
 * parent node (the parent of @p node itself is @p parent).
 *
 * This reach decides whether a non-ANSI direction declaration is backed
 * by its own standalone declaration; the backing-declaration merge
 * (ModuleIONormalizer) and the backed-placeholder exemption
 * (DefaultResolution, ADR-0012 §6) must agree on it, so it has one home.
 */
template <typename Visit>
inline void for_each_standalone_decl(const AST::Node::Ptr &node, const AST::Node::Ptr &parent,
                                     Visit &&visit)
{
    if(!node) {
        return;
    }

    if(node->is_node_type(AST::NodeType::Var) || node->is_node_category(AST::NodeType::Net)) {
        visit(AST::cast_to<AST::Declaration>(node), parent);
        return;
    }

    switch(node->get_node_type()) {
    case AST::NodeType::Port:
    case AST::NodeType::Function:
    case AST::NodeType::Task:
    case AST::NodeType::Initial:
    case AST::NodeType::Always:
    case AST::NodeType::AlwaysFF:
    case AST::NodeType::AlwaysComb:
    case AST::NodeType::AlwaysLatch:
        return;
    default:
        break;
    }

    const AST::Node::ListPtr children = node->get_children();
    for(const AST::Node::Ptr &child : *children) {
        for_each_standalone_decl(child, node, std::forward<Visit>(visit));
    }
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse

#endif
