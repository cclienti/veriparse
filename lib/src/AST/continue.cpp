// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/continue.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Continue::Continue(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Continue);
    set_node_categories({NodeType::Node});
}

Continue &Continue::operator=(const Continue &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Continue::operator=(const Node &rhs)
{
    const Continue &rhs_cast = static_cast<const Continue &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Continue::operator==(const Continue &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Continue::operator==(const Node &rhs) const
{
    const Continue &rhs_cast = static_cast<const Continue &>(rhs);
    return operator==(rhs_cast);
}

bool Continue::operator!=(const Continue &rhs) const { return !(operator==(rhs)); }

bool Continue::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Continue::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Continue::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool Continue::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Continue::ListPtr Continue::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Continue>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Continue::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void Continue::clone_children(Node::Ptr new_node) const {}

Node::Ptr Continue::alloc_same(void) const
{
    Ptr p(new Continue);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Continue &p)
{
    os << "Continue: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Continue::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Continue: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse