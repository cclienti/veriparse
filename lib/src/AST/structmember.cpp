// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/structmember.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

StructMember::StructMember(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::StructMember);
    set_node_categories({NodeType::Node});
}

StructMember::StructMember(const Node::Ptr type, const Length::ListPtr lengths,
                           const Rvalue::Ptr right, const std::string &name,
                           const std::string &filename, uint32_t line)
    : Node(filename, line), m_type(type), m_lengths(lengths), m_right(right), m_name(name)
{
    set_node_type(NodeType::StructMember);
    set_node_categories({NodeType::Node});
}

StructMember &StructMember::operator=(const StructMember &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &StructMember::operator=(const Node &rhs)
{
    const StructMember &rhs_cast = static_cast<const StructMember &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool StructMember::operator==(const StructMember &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool StructMember::operator==(const Node &rhs) const
{
    const StructMember &rhs_cast = static_cast<const StructMember &>(rhs);
    return operator==(rhs_cast);
}

bool StructMember::operator!=(const StructMember &rhs) const { return !(operator==(rhs)); }

bool StructMember::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool StructMember::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool StructMember::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "StructMember::replace matches multiple times (Node::type)";
            }
            set_type(new_node);
            found = true;
        }
    }
    if(get_lengths()) {
        Length::ListPtr new_list = std::make_shared<Length::List>();
        for(const Length::Ptr &lnode : *get_lengths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructMember::replace matches multiple times "
                                       "(list(Length)::lengths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Length>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructMember::replace "
                            << "of children list(Length)::lengths";
            }
        }
        if(new_list->size() != 0) {
            set_lengths(new_list);
        } else {
            set_lengths(nullptr);
        }
    }
    if(get_right()) {
        if(get_right() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "StructMember::replace matches multiple times (Rvalue::right)";
            }
            set_right(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    return found;
}

bool StructMember::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_lengths()) {
        Length::ListPtr new_list = std::make_shared<Length::List>();
        for(const Length::Ptr &lnode : *get_lengths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructMember::replace matches multiple times "
                                       "(list(Length)::lengths)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Length>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructMember::replace "
                            << "of children list(Length)::lengths";
            }
        }
        if(new_list->size() != 0) {
            set_lengths(new_list);
        } else {
            set_lengths(nullptr);
        }
    }
    return found;
}

StructMember::ListPtr StructMember::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<StructMember>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr StructMember::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    if(get_lengths()) {
        for(const Length::Ptr &node : *get_lengths()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_right()) {
        list->push_back(std::static_pointer_cast<Node>(get_right()));
    }
    return list;
}

void StructMember::clone_children(Node::Ptr new_node) const
{
    if(get_type()) {
        cast_to<StructMember>(new_node)->set_type(get_type()->clone());
    }
    cast_to<StructMember>(new_node)->set_lengths(Length::clone_list(get_lengths()));
    if(get_right()) {
        cast_to<StructMember>(new_node)->set_right(cast_to<Rvalue>(get_right()->clone()));
    }
}

Node::Ptr StructMember::alloc_same(void) const
{
    Ptr p(new StructMember);
    return p;
}

std::ostream &operator<<(std::ostream &os, const StructMember &p)
{
    os << "StructMember: {";
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

std::ostream &operator<<(std::ostream &os, const StructMember::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "StructMember: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse