// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/return.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Return::Return(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Return);
    set_node_categories({NodeType::Node});
}

Return::Return(const Node::Ptr value, const std::string &filename, uint32_t line)
    : Node(filename, line), m_value(value)
{
    set_node_type(NodeType::Return);
    set_node_categories({NodeType::Node});
}

Return &Return::operator=(const Return &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Return::operator=(const Node &rhs)
{
    const Return &rhs_cast = static_cast<const Return &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Return::operator==(const Return &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Return::operator==(const Node &rhs) const
{
    const Return &rhs_cast = static_cast<const Return &>(rhs);
    return operator==(rhs_cast);
}

bool Return::operator!=(const Return &rhs) const { return !(operator==(rhs)); }

bool Return::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Return::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Return::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_value()) {
        if(get_value() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Return::replace matches multiple times (Node::value)";
            }
            set_value(new_node);
            found = true;
        }
    }
    return found;
}

bool Return::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Return::ListPtr Return::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Return>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Return::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_value()) {
        list->push_back(std::static_pointer_cast<Node>(get_value()));
    }
    return list;
}

void Return::clone_children(Node::Ptr new_node) const
{
    if(get_value()) {
        cast_to<Return>(new_node)->set_value(get_value()->clone());
    }
}

Node::Ptr Return::alloc_same(void) const
{
    Ptr p(new Return);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Return &p)
{
    os << "Return: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Return::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Return: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse