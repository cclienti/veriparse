// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/assignmentpattern.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

AssignmentPattern::AssignmentPattern(const std::string &filename, uint32_t line)
    : Node(filename, line)
{
    set_node_type(NodeType::AssignmentPattern);
    set_node_categories({NodeType::Node});
}

AssignmentPattern::AssignmentPattern(const Node::ListPtr items, const Node::Ptr times,
                                     const std::string &filename, uint32_t line)
    : Node(filename, line), m_items(items), m_times(times)
{
    set_node_type(NodeType::AssignmentPattern);
    set_node_categories({NodeType::Node});
}

AssignmentPattern &AssignmentPattern::operator=(const AssignmentPattern &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &AssignmentPattern::operator=(const Node &rhs)
{
    const AssignmentPattern &rhs_cast = static_cast<const AssignmentPattern &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool AssignmentPattern::operator==(const AssignmentPattern &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool AssignmentPattern::operator==(const Node &rhs) const
{
    const AssignmentPattern &rhs_cast = static_cast<const AssignmentPattern &>(rhs);
    return operator==(rhs_cast);
}

bool AssignmentPattern::operator!=(const AssignmentPattern &rhs) const
{
    return !(operator==(rhs));
}

bool AssignmentPattern::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool AssignmentPattern::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool AssignmentPattern::replace(Node::Ptr node, Node::Ptr new_node)
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
                        LOG_WARNING << *this << ", "
                                    << "AssignmentPattern::replace matches multiple times "
                                       "(list(Node)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during AssignmentPattern::replace "
                            << "of children list(Node)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
        }
    }
    if(get_times()) {
        if(get_times() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "AssignmentPattern::replace matches multiple times (Node::times)";
            }
            set_times(new_node);
            found = true;
        }
    }
    return found;
}

bool AssignmentPattern::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                        LOG_WARNING << *this << ", "
                                    << "AssignmentPattern::replace matches multiple times "
                                       "(list(Node)::items)";
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
                            << "found an empty node during AssignmentPattern::replace "
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

AssignmentPattern::ListPtr AssignmentPattern::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<AssignmentPattern>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr AssignmentPattern::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_items()) {
        for(const Node::Ptr &node : *get_items()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_times()) {
        list->push_back(std::static_pointer_cast<Node>(get_times()));
    }
    return list;
}

void AssignmentPattern::clone_children(Node::Ptr new_node) const
{
    cast_to<AssignmentPattern>(new_node)->set_items(Node::clone_list(get_items()));
    if(get_times()) {
        cast_to<AssignmentPattern>(new_node)->set_times(get_times()->clone());
    }
}

Node::Ptr AssignmentPattern::alloc_same(void) const
{
    Ptr p(new AssignmentPattern);
    return p;
}

std::ostream &operator<<(std::ostream &os, const AssignmentPattern &p)
{
    os << "AssignmentPattern: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const AssignmentPattern::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "AssignmentPattern: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse