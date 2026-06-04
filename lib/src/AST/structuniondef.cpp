// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/structuniondef.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

StructUnionDef::StructUnionDef(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::StructUnionDef);
    set_node_categories({NodeType::Node});
}

StructUnionDef::StructUnionDef(const StructMember::ListPtr members, const bool &packed,
                               const bool &sign, const std::string &filename, uint32_t line)
    : Node(filename, line), m_members(members), m_packed(packed), m_sign(sign)
{
    set_node_type(NodeType::StructUnionDef);
    set_node_categories({NodeType::Node});
}

StructUnionDef &StructUnionDef::operator=(const StructUnionDef &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_packed(rhs.get_packed());
    set_sign(rhs.get_sign());
    return *this;
}

Node &StructUnionDef::operator=(const Node &rhs)
{
    const StructUnionDef &rhs_cast = static_cast<const StructUnionDef &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool StructUnionDef::operator==(const StructUnionDef &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_packed() != rhs.get_packed()) {
        return false;
    }
    if(get_sign() != rhs.get_sign()) {
        return false;
    }
    return true;
}

bool StructUnionDef::operator==(const Node &rhs) const
{
    const StructUnionDef &rhs_cast = static_cast<const StructUnionDef &>(rhs);
    return operator==(rhs_cast);
}

bool StructUnionDef::operator!=(const StructUnionDef &rhs) const { return !(operator==(rhs)); }

bool StructUnionDef::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool StructUnionDef::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool StructUnionDef::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_members()) {
        StructMember::ListPtr new_list = std::make_shared<StructMember::List>();
        for(const StructMember::Ptr &lnode : *get_members()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructUnionDef::replace matches multiple times "
                                       "(list(StructMember)::members)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<StructMember>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructUnionDef::replace "
                            << "of children list(StructMember)::members";
            }
        }
        if(new_list->size() != 0) {
            set_members(new_list);
        } else {
            set_members(nullptr);
        }
    }
    return found;
}

bool StructUnionDef::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_members()) {
        StructMember::ListPtr new_list = std::make_shared<StructMember::List>();
        for(const StructMember::Ptr &lnode : *get_members()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructUnionDef::replace matches multiple times "
                                       "(list(StructMember)::members)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<StructMember>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructUnionDef::replace "
                            << "of children list(StructMember)::members";
            }
        }
        if(new_list->size() != 0) {
            set_members(new_list);
        } else {
            set_members(nullptr);
        }
    }
    return found;
}

StructUnionDef::ListPtr StructUnionDef::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<StructUnionDef>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr StructUnionDef::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_members()) {
        for(const StructMember::Ptr &node : *get_members()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void StructUnionDef::clone_children(Node::Ptr new_node) const
{
    cast_to<StructUnionDef>(new_node)->set_members(StructMember::clone_list(get_members()));
}

Node::Ptr StructUnionDef::alloc_same(void) const
{
    Ptr p(new StructUnionDef);
    return p;
}

std::ostream &operator<<(std::ostream &os, const StructUnionDef &p)
{
    os << "StructUnionDef: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "packed: " << p.get_packed() << ", ";

    os << "sign: " << p.get_sign();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const StructUnionDef::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "StructUnionDef: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse