// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/analysis/synthesizable_check.hpp>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

namespace
{

/// A virtual interface (IEEE 1800-2017 §25.9) is a dynamic handle — a
/// class/testbench construct with no static hardware. It reaches this pass as an
/// InterfaceType whose is_virtual flag is set (decisive at parse). There is no
/// flattening semantics for it, so reject rather than silently mis-lower. Report
/// every occurrence in the subtree.
///
/// The recursion is written out rather than reusing Analysis::Search: its
/// get_node_list needs a non-empty blacklist pack (Search<> does not compile),
/// and the only ready instantiation (StandardSearch) prunes Function/Task
/// bodies, whereas this check must descend into every node.
int check_virtual_interfaces(const AST::Node::Ptr &node)
{
    if(!node) {
        return 0;
    }

    int violations = 0;

    if(node->is_node_type(AST::NodeType::InterfaceType)) {
        const auto &type = AST::cast_to<AST::InterfaceType>(node);
        if(type->get_is_virtual()) {
            LOG_ERROR_N(node) << "virtual interface '" << type->get_name()
                              << "' is not synthesizable (IEEE 1800-2017 25.9)";
            ++violations;
        }
    }

    // get_children() may return null for leaf nodes, so guard before deref.
    const AST::Node::ListPtr children = node->get_children();
    if(children) {
        for(const AST::Node::Ptr &child : *children) {
            violations += check_virtual_interfaces(child);
        }
    }

    return violations;
}

} // namespace

int SynthesizableCheck::check(const AST::Node::Ptr &node)
{
    if(!node) {
        return 0;
    }

    int violations = 0;
    violations += check_virtual_interfaces(node); // §25.9
    // Future non-synthesizable constructs (classes, clocking blocks, assertions,
    // DPI, …) add their own sub-check here.
    return violations;
}

int SynthesizableCheck::check(const std::vector<AST::Node::Ptr> &sources)
{
    int violations = 0;
    for(const AST::Node::Ptr &node : sources) {
        violations += check(node);
    }
    return violations;
}

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse
