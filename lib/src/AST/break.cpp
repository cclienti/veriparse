// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/break.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Break::Break(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Break);
    set_node_categories({NodeType::Node});
}

Break &Break::operator=(const Break &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Break::operator=(const Node &rhs)
{
    const Break &rhs_cast = static_cast<const Break &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Break::operator==(const Break &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Break::operator==(const Node &rhs) const
{
    const Break &rhs_cast = static_cast<const Break &>(rhs);
    return operator==(rhs_cast);
}

bool Break::operator!=(const Break &rhs) const { return !(operator==(rhs)); }

bool Break::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Break::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Break::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool Break::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Break::ListPtr Break::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Break>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Break::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void Break::clone_children(Node::Ptr new_node) const {}

Node::Ptr Break::alloc_same(void) const
{
    Ptr p(new Break);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Break &p)
{
    os << "Break: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Break::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Break: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse