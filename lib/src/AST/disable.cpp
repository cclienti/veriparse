// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/disable.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Disable::Disable(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Disable);
    set_node_categories({NodeType::Node});
}

Disable::Disable(const Identifier::Ptr dest, const std::string &filename, uint32_t line)
    : Node(filename, line), m_dest(dest)
{
    set_node_type(NodeType::Disable);
    set_node_categories({NodeType::Node});
}

Disable &Disable::operator=(const Disable &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &Disable::operator=(const Node &rhs)
{
    const Disable &rhs_cast = static_cast<const Disable &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Disable::operator==(const Disable &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool Disable::operator==(const Node &rhs) const
{
    const Disable &rhs_cast = static_cast<const Disable &>(rhs);
    return operator==(rhs_cast);
}

bool Disable::operator!=(const Disable &rhs) const { return !(operator==(rhs)); }

bool Disable::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Disable::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Disable::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_dest()) {
        if(get_dest() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Disable::replace matches multiple times (Identifier::dest)";
            }
            set_dest(cast_to<Identifier>(new_node));
            found = true;
        }
    }
    return found;
}

bool Disable::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Disable::ListPtr Disable::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Disable>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Disable::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_dest()) {
        list->push_back(std::static_pointer_cast<Node>(get_dest()));
    }
    return list;
}

void Disable::clone_children(Node::Ptr new_node) const
{
    if(get_dest()) {
        cast_to<Disable>(new_node)->set_dest(cast_to<Identifier>(get_dest()->clone()));
    }
}

Node::Ptr Disable::alloc_same(void) const
{
    Ptr p(new Disable);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Disable &p)
{
    os << "Disable: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Disable::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Disable: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse