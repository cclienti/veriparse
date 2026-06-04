// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/union.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Union::Union(const std::string &filename, uint32_t line) : StructUnionDef(filename, line)
{
    set_node_type(NodeType::Union);
    set_node_categories({NodeType::StructUnionDef, NodeType::Node});
}

Union::Union(const StructMember::ListPtr members, const bool &packed, const bool &sign,
             const std::string &filename, uint32_t line)
    : StructUnionDef(members, packed, sign, filename, line)
{
    set_node_type(NodeType::Union);
    set_node_categories({NodeType::StructUnionDef, NodeType::Node});
}

Union &Union::operator=(const Union &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_packed(rhs.get_packed());
    set_sign(rhs.get_sign());
    return *this;
}

Node &Union::operator=(const Node &rhs)
{
    const Union &rhs_cast = static_cast<const Union &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Union::operator==(const Union &rhs) const
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

bool Union::operator==(const Node &rhs) const
{
    const Union &rhs_cast = static_cast<const Union &>(rhs);
    return operator==(rhs_cast);
}

bool Union::operator!=(const Union &rhs) const { return !(operator==(rhs)); }

bool Union::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Union::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Union::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "Union::replace matches multiple times "
                                       "(list(StructMember)::members)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<StructMember>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Union::replace "
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

bool Union::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "Union::replace matches multiple times "
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
                            << "found an empty node during Union::replace "
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

Union::ListPtr Union::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Union>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Union::get_children(void) const
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

void Union::clone_children(Node::Ptr new_node) const
{
    cast_to<Union>(new_node)->set_members(StructMember::clone_list(get_members()));
}

Node::Ptr Union::alloc_same(void) const
{
    Ptr p(new Union);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Union &p)
{
    os << "Union: {";
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

std::ostream &operator<<(std::ostream &os, const Union::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Union: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse