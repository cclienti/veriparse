// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/package.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Package::Package(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Package);
    set_node_categories({NodeType::Node});
}

Package::Package(const Node::ListPtr items, const std::string &name, const std::string &filename,
                 uint32_t line)
    : Node(filename, line), m_items(items), m_name(name)
{
    set_node_type(NodeType::Package);
    set_node_categories({NodeType::Node});
}

Package &Package::operator=(const Package &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Package::operator=(const Node &rhs)
{
    const Package &rhs_cast = static_cast<const Package &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Package::operator==(const Package &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Package::operator==(const Node &rhs) const
{
    const Package &rhs_cast = static_cast<const Package &>(rhs);
    return operator==(rhs_cast);
}

bool Package::operator!=(const Package &rhs) const { return !(operator==(rhs)); }

bool Package::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Package::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Package::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_items()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Package::replace matches multiple times (list(Node)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Package::replace "
                            << "of children list(Node)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
        }
    }
    return found;
}

bool Package::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_items()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Package::replace matches multiple times (list(Node)::items)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(n);
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Package::replace "
                            << "of children list(Node)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
        }
    }
    return found;
}

Package::ListPtr Package::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Package>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Package::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_items()) {
        for(const Node::Ptr &node : *get_items()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void Package::clone_children(Node::Ptr new_node) const
{
    cast_to<Package>(new_node)->set_items(Node::clone_list(get_items()));
}

Node::Ptr Package::alloc_same(void) const
{
    Ptr p(new Package);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Package &p)
{
    os << "Package: {";
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

std::ostream &operator<<(std::ostream &os, const Package::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Package: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse