// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/uniontype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

UnionType::UnionType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::UnionType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

UnionType::UnionType(const Member::ListPtr members, const Dimension::ListPtr packed_dims,
                     const bool &is_packed, const bool &is_tagged,
                     const DataType::SigningEnum &signing, const std::string &filename,
                     uint32_t line)
    : DataType(packed_dims, signing, filename, line), m_members(members), m_is_packed(is_packed),
      m_is_tagged(is_tagged)
{
    set_node_type(NodeType::UnionType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

UnionType &UnionType::operator=(const UnionType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_packed(rhs.get_is_packed());
    set_is_tagged(rhs.get_is_tagged());
    set_signing(rhs.get_signing());
    return *this;
}

Node &UnionType::operator=(const Node &rhs)
{
    const UnionType &rhs_cast = static_cast<const UnionType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool UnionType::operator==(const UnionType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_packed() != rhs.get_is_packed()) {
        return false;
    }
    if(get_is_tagged() != rhs.get_is_tagged()) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool UnionType::operator==(const Node &rhs) const
{
    const UnionType &rhs_cast = static_cast<const UnionType &>(rhs);
    return operator==(rhs_cast);
}

bool UnionType::operator!=(const UnionType &rhs) const { return !(operator==(rhs)); }

bool UnionType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool UnionType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool UnionType::replace(Node::Ptr node, Node::Ptr new_node)
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
                            << "UnionType::replace matches multiple times (list(Member)::members)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Member>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during UnionType::replace "
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
                                    << "UnionType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during UnionType::replace "
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

bool UnionType::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                            << "UnionType::replace matches multiple times (list(Member)::members)";
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
                            << "found an empty node during UnionType::replace "
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
                                    << "UnionType::replace matches multiple times "
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
                            << "found an empty node during UnionType::replace "
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

UnionType::ListPtr UnionType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<UnionType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr UnionType::get_children(void) const
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

void UnionType::clone_children(Node::Ptr new_node) const
{
    cast_to<UnionType>(new_node)->set_members(Member::clone_list(get_members()));
    cast_to<UnionType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr UnionType::alloc_same(void) const
{
    Ptr p(new UnionType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const UnionType &p)
{
    os << "UnionType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_packed: " << p.get_is_packed() << ", ";

    os << "is_tagged: " << p.get_is_tagged() << ", ";

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const UnionType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "UnionType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse