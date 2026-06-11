// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/passes/transformations/enum_inliner.hpp>
#include <veriparse/AST/enumtype.hpp>
#include <veriparse/AST/enumitem.hpp>
#include <veriparse/AST/identifier.hpp>
#include <veriparse/AST/intconstn.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Transformations
{

int EnumInliner::process(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    // Step 1: collect all enum item names and values
    m_replace_map.clear();
    if(collect_enum_items(node) != 0) {
        return 1;
    }

    if(m_replace_map.empty()) {
        return 0;
    }

    // Step 2: replace matching identifiers
    return replace_identifiers(node, parent);
}

int EnumInliner::collect_enum_items(const AST::Node::Ptr &node)
{
    if(!node) {
        return 0;
    }

    if(node->get_node_type() == AST::NodeType::EnumType) {
        const auto &enumdef = AST::cast_to<AST::EnumType>(node);
        const auto &items = enumdef->get_items();
        if(items) {
            for(const AST::EnumItem::Ptr &item : *items) {
                if(!item) {
                    continue;
                }
                const auto &val = item->get_value();
                if(!val || val->get_node_type() != AST::NodeType::IntConstN) {
                    LOG_ERROR_N(item)
                        << "enum item '" << item->get_name() << "': missing or non-constant value"
                        << " (run EnumElaboration first)";
                    return 1;
                }
                m_replace_map[item->get_name()] = AST::cast_to<AST::IntConstN>(val);
            }
        }
        // Do not recurse into EnumDef children
        return 0;
    }

    AST::Node::ListPtr children = node->get_children();
    for(AST::Node::Ptr &child : *children) {
        if(collect_enum_items(child) != 0) {
            return 1;
        }
    }
    return 0;
}

int EnumInliner::replace_identifiers(AST::Node::Ptr node, AST::Node::Ptr parent)
{
    if(!node) {
        return 0;
    }

    // Skip EnumDef subtrees — don't replace names inside the definition
    if(node->get_node_type() == AST::NodeType::EnumType) {
        return 0;
    }

    if(node->get_node_type() == AST::NodeType::Identifier) {
        const auto &id = AST::cast_to<AST::Identifier>(node);
        // Only replace simple identifiers: no hierarchical '.' path (hier) and no
        // SV '::' scope. An enum item is a local name; a qualified reference names
        // something in another scope/namespace and must not be inlined.
        if(!id->get_hier() && !id->get_scope()) {
            auto it = m_replace_map.find(id->get_name());
            if(it != m_replace_map.end() && parent) {
                auto replacement = AST::cast_to<AST::IntConstN>(it->second->clone());
                parent->replace(node, replacement);
                return 0;
            }
        }
    }

    AST::Node::ListPtr children = node->get_children();
    for(AST::Node::Ptr &child : *children) {
        if(replace_identifiers(child, node) != 0) {
            return 1;
        }
    }
    return 0;
}

} // namespace Transformations
} // namespace Passes
} // namespace Veriparse
