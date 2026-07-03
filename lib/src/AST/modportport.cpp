// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/modportport.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ModportPort::ModportPort(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::ModportPort);
    set_node_categories({NodeType::Node});
}

ModportPort::ModportPort(const std::string &name, const DirectionEnum &direction,
                         const std::string &filename, uint32_t line)
    : Node(filename, line), m_name(name), m_direction(direction)
{
    set_node_type(NodeType::ModportPort);
    set_node_categories({NodeType::Node});
}

ModportPort &ModportPort::operator=(const ModportPort &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_direction(rhs.get_direction());
    return *this;
}

Node &ModportPort::operator=(const Node &rhs)
{
    const ModportPort &rhs_cast = static_cast<const ModportPort &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ModportPort::operator==(const ModportPort &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    if(get_direction() != rhs.get_direction()) {
        return false;
    }
    return true;
}

bool ModportPort::operator==(const Node &rhs) const
{
    const ModportPort &rhs_cast = static_cast<const ModportPort &>(rhs);
    return operator==(rhs_cast);
}

bool ModportPort::operator!=(const ModportPort &rhs) const { return !(operator==(rhs)); }

bool ModportPort::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ModportPort::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ModportPort::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool ModportPort::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

ModportPort::ListPtr ModportPort::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ModportPort>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ModportPort::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void ModportPort::clone_children(Node::Ptr new_node) const {}

Node::Ptr ModportPort::alloc_same(void) const
{
    Ptr p(new ModportPort);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ModportPort &p)
{
    os << "ModportPort: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "direction: " << p.get_direction();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ModportPort::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ModportPort: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const ModportPort::DirectionEnum p)
{
    switch(p) {
    case ModportPort::DirectionEnum::INPUT:
        os << "INPUT";
        break;
    case ModportPort::DirectionEnum::OUTPUT:
        os << "OUTPUT";
        break;
    case ModportPort::DirectionEnum::INOUT:
        os << "INOUT";
        break;
    case ModportPort::DirectionEnum::REF:
        os << "REF";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse