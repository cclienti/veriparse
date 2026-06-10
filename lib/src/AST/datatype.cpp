// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/datatype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

DataType::DataType(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::DataType);
    set_node_categories({NodeType::Node});
}

DataType::DataType(const Dimension::ListPtr packed_dims, const SigningEnum &signing,
                   const std::string &filename, uint32_t line)
    : Node(filename, line), m_packed_dims(packed_dims), m_signing(signing)
{
    set_node_type(NodeType::DataType);
    set_node_categories({NodeType::Node});
}

DataType &DataType::operator=(const DataType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_signing(rhs.get_signing());
    return *this;
}

Node &DataType::operator=(const Node &rhs)
{
    const DataType &rhs_cast = static_cast<const DataType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool DataType::operator==(const DataType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool DataType::operator==(const Node &rhs) const
{
    const DataType &rhs_cast = static_cast<const DataType &>(rhs);
    return operator==(rhs_cast);
}

bool DataType::operator!=(const DataType &rhs) const { return !(operator==(rhs)); }

bool DataType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool DataType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool DataType::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "DataType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during DataType::replace "
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

bool DataType::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "DataType::replace matches multiple times "
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
                            << "found an empty node during DataType::replace "
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

DataType::ListPtr DataType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<DataType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr DataType::get_children(void) const
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

void DataType::clone_children(Node::Ptr new_node) const
{
    cast_to<DataType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr DataType::alloc_same(void) const
{
    Ptr p(new DataType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const DataType &p)
{
    os << "DataType: {";
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

std::ostream &operator<<(std::ostream &os, const DataType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "DataType: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const DataType::SigningEnum p)
{
    switch(p) {
    case DataType::SigningEnum::NONE:
        os << "NONE";
        break;
    case DataType::SigningEnum::SIGNED:
        os << "SIGNED";
        break;
    case DataType::SigningEnum::UNSIGNED:
        os << "UNSIGNED";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse