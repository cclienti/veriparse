// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/strength.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Strength::Strength(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Strength);
    set_node_categories({NodeType::Node});
}

Strength &Strength::operator=(const Strength &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Strength::operator=(const Node &rhs)
{
    const Strength &rhs_cast = static_cast<const Strength &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Strength::operator==(const Strength &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Strength::operator==(const Node &rhs) const
{
    const Strength &rhs_cast = static_cast<const Strength &>(rhs);
    return operator==(rhs_cast);
}

bool Strength::operator!=(const Strength &rhs) const { return !(operator==(rhs)); }

bool Strength::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Strength::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Strength::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool Strength::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Strength::ListPtr Strength::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Strength>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Strength::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void Strength::clone_children(Node::Ptr new_node) const {}

Node::Ptr Strength::alloc_same(void) const
{
    Ptr p(new Strength);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Strength &p)
{
    os << "Strength: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Strength::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Strength: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse