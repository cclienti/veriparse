// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Dimension::Dimension(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Dimension);
    set_node_categories({NodeType::Node});
}

Dimension &Dimension::operator=(const Dimension &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Dimension::operator=(const Node &rhs)
{
    const Dimension &rhs_cast = static_cast<const Dimension &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Dimension::operator==(const Dimension &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Dimension::operator==(const Node &rhs) const
{
    const Dimension &rhs_cast = static_cast<const Dimension &>(rhs);
    return operator==(rhs_cast);
}

bool Dimension::operator!=(const Dimension &rhs) const { return !(operator==(rhs)); }

bool Dimension::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Dimension::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Dimension::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool Dimension::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Dimension::ListPtr Dimension::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Dimension>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Dimension::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void Dimension::clone_children(Node::Ptr new_node) const {}

Node::Ptr Dimension::alloc_same(void) const
{
    Ptr p(new Dimension);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Dimension &p)
{
    os << "Dimension: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Dimension::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Dimension: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse