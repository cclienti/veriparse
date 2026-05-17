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

StructMember::StructMember(const Width::ListPtr widths, const Node::Ptr type,
                           const std::string &name, const bool &sign, const std::string &filename,
                           uint32_t line)
    : Node(filename, line), m_widths(widths), m_type(type), m_name(name), m_sign(sign)
{
    set_node_type(NodeType::StructMember);
    set_node_categories({NodeType::Node});
}

StructMember &StructMember::operator=(const StructMember &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_sign(rhs.get_sign());
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
    if(get_sign() != rhs.get_sign()) {
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
    if(get_widths()) {
        Width::ListPtr new_list = std::make_shared<Width::List>();
        for(const Width::Ptr &lnode : *get_widths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "StructMember::replace matches multiple times (list(Width)::widths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Width>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructMember::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
        }
    }
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
    return found;
}

bool StructMember::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_widths()) {
        Width::ListPtr new_list = std::make_shared<Width::List>();
        for(const Width::Ptr &lnode : *get_widths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "StructMember::replace matches multiple times (list(Width)::widths)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes)
                            new_list->push_back(cast_to<Width>(n));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructMember::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
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
    if(get_widths()) {
        for(const Width::Ptr &node : *get_widths()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void StructMember::clone_children(Node::Ptr new_node) const
{
    cast_to<StructMember>(new_node)->set_widths(Width::clone_list(get_widths()));
    if(get_type()) {
        cast_to<StructMember>(new_node)->set_type(get_type()->clone());
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

    if(!p.get_filename().empty())
        os << ", ";

    os << "name: " << p.get_name() << ", ";

    os << "sign: " << p.get_sign();
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