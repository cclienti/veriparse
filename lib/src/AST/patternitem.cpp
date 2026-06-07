// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/patternitem.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

PatternItem::PatternItem(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::PatternItem);
    set_node_categories({NodeType::Node});
}

PatternItem::PatternItem(const Node::Ptr key, const Node::Ptr value, const bool &is_default,
                         const std::string &filename, uint32_t line)
    : Node(filename, line), m_key(key), m_value(value), m_is_default(is_default)
{
    set_node_type(NodeType::PatternItem);
    set_node_categories({NodeType::Node});
}

PatternItem &PatternItem::operator=(const PatternItem &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_default(rhs.get_is_default());
    return *this;
}

Node &PatternItem::operator=(const Node &rhs)
{
    const PatternItem &rhs_cast = static_cast<const PatternItem &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool PatternItem::operator==(const PatternItem &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_default() != rhs.get_is_default()) {
        return false;
    }
    return true;
}

bool PatternItem::operator==(const Node &rhs) const
{
    const PatternItem &rhs_cast = static_cast<const PatternItem &>(rhs);
    return operator==(rhs_cast);
}

bool PatternItem::operator!=(const PatternItem &rhs) const { return !(operator==(rhs)); }

bool PatternItem::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool PatternItem::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool PatternItem::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_key()) {
        if(get_key() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "PatternItem::replace matches multiple times (Node::key)";
            }
            set_key(new_node);
            found = true;
        }
    }
    if(get_value()) {
        if(get_value() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "PatternItem::replace matches multiple times (Node::value)";
            }
            set_value(new_node);
            found = true;
        }
    }
    return found;
}

bool PatternItem::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

PatternItem::ListPtr PatternItem::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<PatternItem>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr PatternItem::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_key()) {
        list->push_back(std::static_pointer_cast<Node>(get_key()));
    }
    if(get_value()) {
        list->push_back(std::static_pointer_cast<Node>(get_value()));
    }
    return list;
}

void PatternItem::clone_children(Node::Ptr new_node) const
{
    if(get_key()) {
        cast_to<PatternItem>(new_node)->set_key(get_key()->clone());
    }
    if(get_value()) {
        cast_to<PatternItem>(new_node)->set_value(get_value()->clone());
    }
}

Node::Ptr PatternItem::alloc_same(void) const
{
    Ptr p(new PatternItem);
    return p;
}

std::ostream &operator<<(std::ostream &os, const PatternItem &p)
{
    os << "PatternItem: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_default: " << p.get_is_default();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const PatternItem::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "PatternItem: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse