// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/bytetype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ByteType::ByteType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::ByteType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

ByteType::ByteType(const Dimension::ListPtr packed_dims, const DataType::SigningEnum &signing,
                   const std::string &filename, uint32_t line)
    : DataType(packed_dims, signing, filename, line)
{
    set_node_type(NodeType::ByteType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

ByteType &ByteType::operator=(const ByteType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_signing(rhs.get_signing());
    return *this;
}

Node &ByteType::operator=(const Node &rhs)
{
    const ByteType &rhs_cast = static_cast<const ByteType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ByteType::operator==(const ByteType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool ByteType::operator==(const Node &rhs) const
{
    const ByteType &rhs_cast = static_cast<const ByteType &>(rhs);
    return operator==(rhs_cast);
}

bool ByteType::operator!=(const ByteType &rhs) const { return !(operator==(rhs)); }

bool ByteType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ByteType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ByteType::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "ByteType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during ByteType::replace "
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

bool ByteType::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "ByteType::replace matches multiple times "
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
                            << "found an empty node during ByteType::replace "
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

ByteType::ListPtr ByteType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ByteType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ByteType::get_children(void) const
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

void ByteType::clone_children(Node::Ptr new_node) const
{
    cast_to<ByteType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr ByteType::alloc_same(void) const
{
    Ptr p(new ByteType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ByteType &p)
{
    os << "ByteType: {";
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

std::ostream &operator<<(std::ostream &os, const ByteType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ByteType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse