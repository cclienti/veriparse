// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_JUMP_HELPERS
#define VERIPARSE_PASSES_TRANSFORMATIONS_JUMP_HELPERS

#include <veriparse/AST/nodes.hpp>
#include <veriparse/AST/node_cast.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

/// The statement a jump wrapper holds (unwrapping a SingleStatement), or the node
/// itself. Jumps are parsed wrapped in a SingleStatement but may appear bare after
/// a transformation.
inline AST::Node::Ptr jump_inner_statement(const AST::Node::Ptr &node)
{
    if(node && node->is_node_type(AST::NodeType::SingleStatement)) {
        return AST::cast_to<AST::SingleStatement>(node)->get_statement();
    }
    return node;
}

/// True if @p node is (or wraps) a jump statement (return/break/continue, §12.8).
inline bool is_jump_statement(const AST::Node::Ptr &node)
{
    const AST::Node::Ptr inner = jump_inner_statement(node);
    return inner && (inner->is_node_type(AST::NodeType::Break) ||
                     inner->is_node_type(AST::NodeType::Continue) ||
                     inner->is_node_type(AST::NodeType::Return));
}

/// True if @p node is (or wraps) a jump statement of the given node type.
inline bool is_jump_statement(const AST::Node::Ptr &node, AST::NodeType type)
{
    const AST::Node::Ptr inner = jump_inner_statement(node);
    return inner && inner->is_node_type(type);
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse

#endif
