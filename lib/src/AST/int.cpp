// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/int.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Int::Int(const std::string &filename, uint32_t line) : Variable(filename, line)
{
    set_node_type(NodeType::Int);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

Int::Int(const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign,
         const std::string &name, const std::string &filename, uint32_t line)
    : Variable(lengths, right, name, filename, line), m_sign(sign)
{
    set_node_type(NodeType::Int);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

Int &Int::operator=(const Int &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_sign(rhs.get_sign());
    set_name(rhs.get_name());
    return *this;
}

Node &Int::operator=(const Node &rhs)
{
    const Int &rhs_cast = static_cast<const Int &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Int::operator==(const Int &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_sign() != rhs.get_sign()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Int::operator==(const Node &rhs) const
{
    const Int &rhs_cast = static_cast<const Int &>(rhs);
    return operator==(rhs_cast);
}

bool Int::operator!=(const Int &rhs) const { return !(operator==(rhs)); }

bool Int::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Int::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Int::replace(Node::Ptr node, Node::Ptr new_node)
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
                        LOG_WARNING
                            << *this << ", "
                            << "Int::replace matches multiple times (list(Length)::lengths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Length>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Int::replace "
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
                            << "Int::replace matches multiple times (Rvalue::right)";
            }
            set_right(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    return found;
}

bool Int::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                        LOG_WARNING
                            << *this << ", "
                            << "Int::replace matches multiple times (list(Length)::lengths)";
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
                            << "found an empty node during Int::replace "
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

Int::ListPtr Int::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Int>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Int::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
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

void Int::clone_children(Node::Ptr new_node) const
{
    cast_to<Int>(new_node)->set_lengths(Length::clone_list(get_lengths()));
    if(get_right()) {
        cast_to<Int>(new_node)->set_right(cast_to<Rvalue>(get_right()->clone()));
    }
}

Node::Ptr Int::alloc_same(void) const
{
    Ptr p(new Int);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Int &p)
{
    os << "Int: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "sign: " << p.get_sign() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Int::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Int: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse