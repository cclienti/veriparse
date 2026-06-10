// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/structtype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

StructType::StructType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::StructType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

StructType::StructType(const Member::ListPtr members, const Dimension::ListPtr packed_dims,
                       const bool &is_packed, const DataType::SigningEnum &signing,
                       const std::string &filename, uint32_t line)
    : DataType(packed_dims, signing, filename, line), m_members(members), m_is_packed(is_packed)
{
    set_node_type(NodeType::StructType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

StructType &StructType::operator=(const StructType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_packed(rhs.get_is_packed());
    set_signing(rhs.get_signing());
    return *this;
}

Node &StructType::operator=(const Node &rhs)
{
    const StructType &rhs_cast = static_cast<const StructType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool StructType::operator==(const StructType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_packed() != rhs.get_is_packed()) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool StructType::operator==(const Node &rhs) const
{
    const StructType &rhs_cast = static_cast<const StructType &>(rhs);
    return operator==(rhs_cast);
}

bool StructType::operator!=(const StructType &rhs) const { return !(operator==(rhs)); }

bool StructType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool StructType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool StructType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_members()) {
        Member::ListPtr new_list = std::make_shared<Member::List>();
        for(const Member::Ptr &lnode : *get_members()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "StructType::replace matches multiple times (list(Member)::members)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Member>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructType::replace "
                            << "of children list(Member)::members";
            }
        }
        if(new_list->size() != 0) {
            set_members(new_list);
        } else {
            set_members(nullptr);
        }
    }
    if(get_packed_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_packed_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructType::replace "
                            << "of children list(Dimension)::packed_dims";
            }
        }
        if(new_list->size() != 0) {
            set_packed_dims(new_list);
        } else {
            set_packed_dims(nullptr);
        }
    }
    return found;
}

bool StructType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_members()) {
        Member::ListPtr new_list = std::make_shared<Member::List>();
        for(const Member::Ptr &lnode : *get_members()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "StructType::replace matches multiple times (list(Member)::members)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Member>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructType::replace "
                            << "of children list(Member)::members";
            }
        }
        if(new_list->size() != 0) {
            set_members(new_list);
        } else {
            set_members(nullptr);
        }
    }
    if(get_packed_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_packed_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "StructType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Dimension>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during StructType::replace "
                            << "of children list(Dimension)::packed_dims";
            }
        }
        if(new_list->size() != 0) {
            set_packed_dims(new_list);
        } else {
            set_packed_dims(nullptr);
        }
    }
    return found;
}

StructType::ListPtr StructType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<StructType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr StructType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_members()) {
        for(const Member::Ptr &node : *get_members()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_packed_dims()) {
        for(const Dimension::Ptr &node : *get_packed_dims()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void StructType::clone_children(Node::Ptr new_node) const
{
    cast_to<StructType>(new_node)->set_members(Member::clone_list(get_members()));
    cast_to<StructType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr StructType::alloc_same(void) const
{
    Ptr p(new StructType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const StructType &p)
{
    os << "StructType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_packed: " << p.get_is_packed() << ", ";

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const StructType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "StructType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse