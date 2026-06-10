// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/cast.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Cast::Cast(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Cast);
    set_node_categories({NodeType::Node});
}

Cast::Cast(const Node::Ptr expr, const std::string &filename, uint32_t line)
    : Node(filename, line), m_expr(expr)
{
    set_node_type(NodeType::Cast);
    set_node_categories({NodeType::Node});
}

Cast &Cast::operator=(const Cast &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Cast::operator=(const Node &rhs)
{
    const Cast &rhs_cast = static_cast<const Cast &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Cast::operator==(const Cast &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Cast::operator==(const Node &rhs) const
{
    const Cast &rhs_cast = static_cast<const Cast &>(rhs);
    return operator==(rhs_cast);
}

bool Cast::operator!=(const Cast &rhs) const { return !(operator==(rhs)); }

bool Cast::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Cast::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Cast::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Cast::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool Cast::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Cast::ListPtr Cast::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Cast>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Cast::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void Cast::clone_children(Node::Ptr new_node) const
{
    if(get_expr()) {
        cast_to<Cast>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr Cast::alloc_same(void) const
{
    Ptr p(new Cast);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Cast &p)
{
    os << "Cast: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Cast::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Cast: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse