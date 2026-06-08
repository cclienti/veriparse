// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/typedef.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Typedef::Typedef(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Typedef);
    set_node_categories({NodeType::Node});
}

Typedef::Typedef(const Node::Ptr def, const std::string &name, const std::string &filename,
                 uint32_t line)
    : Node(filename, line), m_def(def), m_name(name)
{
    set_node_type(NodeType::Typedef);
    set_node_categories({NodeType::Node});
}

Typedef &Typedef::operator=(const Typedef &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Typedef::operator=(const Node &rhs)
{
    const Typedef &rhs_cast = static_cast<const Typedef &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Typedef::operator==(const Typedef &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Typedef::operator==(const Node &rhs) const
{
    const Typedef &rhs_cast = static_cast<const Typedef &>(rhs);
    return operator==(rhs_cast);
}

bool Typedef::operator!=(const Typedef &rhs) const { return !(operator==(rhs)); }

bool Typedef::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Typedef::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Typedef::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_def()) {
        if(get_def() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Typedef::replace matches multiple times (Node::def)";
            }
            set_def(new_node);
            found = true;
        }
    }
    return found;
}

bool Typedef::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Typedef::ListPtr Typedef::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Typedef>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Typedef::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_def()) {
        list->push_back(std::static_pointer_cast<Node>(get_def()));
    }
    return list;
}

void Typedef::clone_children(Node::Ptr new_node) const
{
    if(get_def()) {
        cast_to<Typedef>(new_node)->set_def(get_def()->clone());
    }
}

Node::Ptr Typedef::alloc_same(void) const
{
    Ptr p(new Typedef);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Typedef &p)
{
    os << "Typedef: {";
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

std::ostream &operator<<(std::ostream &os, const Typedef::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Typedef: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse