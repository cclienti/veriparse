// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/port.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Port::Port(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Port);
    set_node_categories({NodeType::Node});
}

Port::Port(const Declaration::Ptr decl, const Node::Ptr expr, const std::string &name,
           const DirectionEnum &direction, const std::string &filename, uint32_t line)
    : Node(filename, line), m_decl(decl), m_expr(expr), m_name(name), m_direction(direction)
{
    set_node_type(NodeType::Port);
    set_node_categories({NodeType::Node});
}

Port &Port::operator=(const Port &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_direction(rhs.get_direction());
    return *this;
}

Node &Port::operator=(const Node &rhs)
{
    const Port &rhs_cast = static_cast<const Port &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Port::operator==(const Port &rhs) const
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

bool Port::operator==(const Node &rhs) const
{
    const Port &rhs_cast = static_cast<const Port &>(rhs);
    return operator==(rhs_cast);
}

bool Port::operator!=(const Port &rhs) const { return !(operator==(rhs)); }

bool Port::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Port::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Port::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_decl()) {
        if(get_decl() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Port::replace matches multiple times (Declaration::decl)";
            }
            set_decl(cast_to<Declaration>(new_node));
            found = true;
        }
    }
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Port::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool Port::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Port::ListPtr Port::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Port>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Port::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_decl()) {
        list->push_back(std::static_pointer_cast<Node>(get_decl()));
    }
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void Port::clone_children(Node::Ptr new_node) const
{
    if(get_decl()) {
        cast_to<Port>(new_node)->set_decl(cast_to<Declaration>(get_decl()->clone()));
    }
    if(get_expr()) {
        cast_to<Port>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr Port::alloc_same(void) const
{
    Ptr p(new Port);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Port &p)
{
    os << "Port: {";
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

std::ostream &operator<<(std::ostream &os, const Port::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Port: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Port::DirectionEnum p)
{
    switch(p) {
    case Port::DirectionEnum::NONE:
        os << "NONE";
        break;
    case Port::DirectionEnum::INPUT:
        os << "INPUT";
        break;
    case Port::DirectionEnum::OUTPUT:
        os << "OUTPUT";
        break;
    case Port::DirectionEnum::INOUT:
        os << "INOUT";
        break;
    case Port::DirectionEnum::REF:
        os << "REF";
        break;
    case Port::DirectionEnum::CONST_REF:
        os << "CONST_REF";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse