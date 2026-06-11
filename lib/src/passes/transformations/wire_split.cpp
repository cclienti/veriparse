// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/wire_split.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

int WireSplit::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    // Check if this is a Wire with an inline right-hand side
    if(node->get_node_type() == AST::NodeType::WireNet) {
        auto wire = AST::cast_to<AST::WireNet>(node);
        auto rhs = wire->get_cont_assign();

        if(rhs && parent) {
            LOG_DEBUG_N(wire) << "Splitting inline wire declaration: " << wire->get_name();

            // Clear the rhs from the wire declaration
            wire->set_cont_assign(nullptr);

            // Build: assign <name> = <rhs>;
            auto lvalue = std::make_shared<AST::Lvalue>(
                std::make_shared<AST::Identifier>(nullptr, nullptr, wire->get_name(),
                                                  wire->get_filename(), wire->get_line()),
                wire->get_filename(), wire->get_line());

            auto assign = std::make_shared<AST::Assign>(lvalue, rhs, nullptr, nullptr,
                                                        wire->get_filename(), wire->get_line());

            // Replace node with [wire, assign]
            auto stmts = std::make_shared<AST::Node::List>();
            stmts->push_back(wire);
            stmts->push_back(assign);
            parent->replace(node, stmts);

            return 0;
        }
    }

    return recurse_in_childs(node);
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
