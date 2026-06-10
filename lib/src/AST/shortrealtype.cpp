// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/shortrealtype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ShortrealType::ShortrealType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::ShortrealType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

ShortrealType::ShortrealType(const Dimension::ListPtr packed_dims,
                             const DataType::SigningEnum &signing, const std::string &filename,
                             uint32_t line)
    : DataType(packed_dims, signing, filename, line)
{
    set_node_type(NodeType::ShortrealType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

ShortrealType &ShortrealType::operator=(const ShortrealType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_signing(rhs.get_signing());
    return *this;
}

Node &ShortrealType::operator=(const Node &rhs)
{
    const ShortrealType &rhs_cast = static_cast<const ShortrealType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ShortrealType::operator==(const ShortrealType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool ShortrealType::operator==(const Node &rhs) const
{
    const ShortrealType &rhs_cast = static_cast<const ShortrealType &>(rhs);
    return operator==(rhs_cast);
}

bool ShortrealType::operator!=(const ShortrealType &rhs) const { return !(operator==(rhs)); }

bool ShortrealType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ShortrealType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ShortrealType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_packed_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_packed_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "ShortrealType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during ShortrealType::replace "
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

bool ShortrealType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_packed_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_packed_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "ShortrealType::replace matches multiple times "
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
                            << "found an empty node during ShortrealType::replace "
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

ShortrealType::ListPtr ShortrealType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ShortrealType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ShortrealType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_packed_dims()) {
        for(const Dimension::Ptr &node : *get_packed_dims()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void ShortrealType::clone_children(Node::Ptr new_node) const
{
    cast_to<ShortrealType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr ShortrealType::alloc_same(void) const
{
    Ptr p(new ShortrealType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ShortrealType &p)
{
    os << "ShortrealType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ShortrealType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ShortrealType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse