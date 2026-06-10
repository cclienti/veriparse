// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/enumitem.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

EnumItem::EnumItem(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::EnumItem);
    set_node_categories({NodeType::Node});
}

EnumItem::EnumItem(const Node::Ptr value, const Dimension::Ptr range, const std::string &name,
                   const std::string &filename, uint32_t line)
    : Node(filename, line), m_value(value), m_range(range), m_name(name)
{
    set_node_type(NodeType::EnumItem);
    set_node_categories({NodeType::Node});
}

EnumItem &EnumItem::operator=(const EnumItem &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &EnumItem::operator=(const Node &rhs)
{
    const EnumItem &rhs_cast = static_cast<const EnumItem &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool EnumItem::operator==(const EnumItem &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool EnumItem::operator==(const Node &rhs) const
{
    const EnumItem &rhs_cast = static_cast<const EnumItem &>(rhs);
    return operator==(rhs_cast);
}

bool EnumItem::operator!=(const EnumItem &rhs) const { return !(operator==(rhs)); }

bool EnumItem::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool EnumItem::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool EnumItem::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_value()) {
        if(get_value() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "EnumItem::replace matches multiple times (Node::value)";
            }
            set_value(new_node);
            found = true;
        }
    }
    if(get_range()) {
        if(get_range() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "EnumItem::replace matches multiple times (Dimension::range)";
            }
            set_range(cast_to<Dimension>(new_node));
            found = true;
        }
    }
    return found;
}

bool EnumItem::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

EnumItem::ListPtr EnumItem::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<EnumItem>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr EnumItem::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_value()) {
        list->push_back(std::static_pointer_cast<Node>(get_value()));
    }
    if(get_range()) {
        list->push_back(std::static_pointer_cast<Node>(get_range()));
    }
    return list;
}

void EnumItem::clone_children(Node::Ptr new_node) const
{
    if(get_value()) {
        cast_to<EnumItem>(new_node)->set_value(get_value()->clone());
    }
    if(get_range()) {
        cast_to<EnumItem>(new_node)->set_range(cast_to<Dimension>(get_range()->clone()));
    }
}

Node::Ptr EnumItem::alloc_same(void) const
{
    Ptr p(new EnumItem);
    return p;
}

std::ostream &operator<<(std::ostream &os, const EnumItem &p)
{
    os << "EnumItem: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const EnumItem::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "EnumItem: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse