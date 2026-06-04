// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/shortint.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Shortint::Shortint(const std::string &filename, uint32_t line) : Variable(filename, line)
{
    set_node_type(NodeType::Shortint);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

Shortint::Shortint(const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign,
                   const std::string &name, const std::string &filename, uint32_t line)
    : Variable(lengths, right, name, filename, line), m_sign(sign)
{
    set_node_type(NodeType::Shortint);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

Shortint &Shortint::operator=(const Shortint &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_sign(rhs.get_sign());
    set_name(rhs.get_name());
    return *this;
}

Node &Shortint::operator=(const Node &rhs)
{
    const Shortint &rhs_cast = static_cast<const Shortint &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Shortint::operator==(const Shortint &rhs) const
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

bool Shortint::operator==(const Node &rhs) const
{
    const Shortint &rhs_cast = static_cast<const Shortint &>(rhs);
    return operator==(rhs_cast);
}

bool Shortint::operator!=(const Shortint &rhs) const { return !(operator==(rhs)); }

bool Shortint::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Shortint::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Shortint::replace(Node::Ptr node, Node::Ptr new_node)
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
                            << "Shortint::replace matches multiple times (list(Length)::lengths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Length>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Shortint::replace "
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
                            << "Shortint::replace matches multiple times (Rvalue::right)";
            }
            set_right(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    return found;
}

bool Shortint::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                            << "Shortint::replace matches multiple times (list(Length)::lengths)";
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
                            << "found an empty node during Shortint::replace "
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

Shortint::ListPtr Shortint::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Shortint>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Shortint::get_children(void) const
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

void Shortint::clone_children(Node::Ptr new_node) const
{
    cast_to<Shortint>(new_node)->set_lengths(Length::clone_list(get_lengths()));
    if(get_right()) {
        cast_to<Shortint>(new_node)->set_right(cast_to<Rvalue>(get_right()->clone()));
    }
}

Node::Ptr Shortint::alloc_same(void) const
{
    Ptr p(new Shortint);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Shortint &p)
{
    os << "Shortint: {";
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

std::ostream &operator<<(std::ostream &os, const Shortint::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Shortint: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse