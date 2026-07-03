// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/modport.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Modport::Modport(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Modport);
    set_node_categories({NodeType::Node});
}

Modport::Modport(const ModportPort::ListPtr ports, const std::string &name,
                 const std::string &filename, uint32_t line)
    : Node(filename, line), m_ports(ports), m_name(name)
{
    set_node_type(NodeType::Modport);
    set_node_categories({NodeType::Node});
}

Modport &Modport::operator=(const Modport &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Modport::operator=(const Node &rhs)
{
    const Modport &rhs_cast = static_cast<const Modport &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Modport::operator==(const Modport &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Modport::operator==(const Node &rhs) const
{
    const Modport &rhs_cast = static_cast<const Modport &>(rhs);
    return operator==(rhs_cast);
}

bool Modport::operator!=(const Modport &rhs) const { return !(operator==(rhs)); }

bool Modport::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Modport::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Modport::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_ports()) {
        ModportPort::ListPtr new_list = std::make_shared<ModportPort::List>();
        for(const ModportPort::Ptr &lnode : *get_ports()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Modport::replace matches multiple times (list(ModportPort)::ports)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ModportPort>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Modport::replace "
                            << "of children list(ModportPort)::ports";
            }
        }
        if(new_list->size() != 0) {
            set_ports(new_list);
        } else {
            set_ports(nullptr);
        }
    }
    return found;
}

bool Modport::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_ports()) {
        ModportPort::ListPtr new_list = std::make_shared<ModportPort::List>();
        for(const ModportPort::Ptr &lnode : *get_ports()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Modport::replace matches multiple times (list(ModportPort)::ports)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<ModportPort>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Modport::replace "
                            << "of children list(ModportPort)::ports";
            }
        }
        if(new_list->size() != 0) {
            set_ports(new_list);
        } else {
            set_ports(nullptr);
        }
    }
    return found;
}

Modport::ListPtr Modport::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Modport>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Modport::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_ports()) {
        for(const ModportPort::Ptr &node : *get_ports()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void Modport::clone_children(Node::Ptr new_node) const
{
    cast_to<Modport>(new_node)->set_ports(ModportPort::clone_list(get_ports()));
}

Node::Ptr Modport::alloc_same(void) const
{
    Ptr p(new Modport);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Modport &p)
{
    os << "Modport: {";
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

std::ostream &operator<<(std::ostream &os, const Modport::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Modport: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse