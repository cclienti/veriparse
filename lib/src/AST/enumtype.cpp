// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/enumtype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

EnumType::EnumType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::EnumType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

EnumType::EnumType(const DataType::Ptr base, const EnumItem::ListPtr items,
                   const Dimension::ListPtr packed_dims, const DataType::SigningEnum &signing,
                   const std::string &filename, uint32_t line)
    : DataType(packed_dims, signing, filename, line), m_base(base), m_items(items)
{
    set_node_type(NodeType::EnumType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

EnumType &EnumType::operator=(const EnumType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_signing(rhs.get_signing());
    return *this;
}

Node &EnumType::operator=(const Node &rhs)
{
    const EnumType &rhs_cast = static_cast<const EnumType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool EnumType::operator==(const EnumType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool EnumType::operator==(const Node &rhs) const
{
    const EnumType &rhs_cast = static_cast<const EnumType &>(rhs);
    return operator==(rhs_cast);
}

bool EnumType::operator!=(const EnumType &rhs) const { return !(operator==(rhs)); }

bool EnumType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool EnumType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool EnumType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_base()) {
        if(get_base() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "EnumType::replace matches multiple times (DataType::base)";
            }
            set_base(cast_to<DataType>(new_node));
            found = true;
        }
    }
    if(get_items()) {
        EnumItem::ListPtr new_list = std::make_shared<EnumItem::List>();
        for(const EnumItem::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "EnumType::replace matches multiple times (list(EnumItem)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<EnumItem>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumType::replace "
                            << "of children list(EnumItem)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
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
                                    << "EnumType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumType::replace "
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

bool EnumType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_items()) {
        EnumItem::ListPtr new_list = std::make_shared<EnumItem::List>();
        for(const EnumItem::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "EnumType::replace matches multiple times (list(EnumItem)::items)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<EnumItem>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumType::replace "
                            << "of children list(EnumItem)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
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
                                    << "EnumType::replace matches multiple times "
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
                            << "found an empty node during EnumType::replace "
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

EnumType::ListPtr EnumType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<EnumType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr EnumType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_base()) {
        list->push_back(std::static_pointer_cast<Node>(get_base()));
    }
    if(get_items()) {
        for(const EnumItem::Ptr &node : *get_items()) {
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

void EnumType::clone_children(Node::Ptr new_node) const
{
    if(get_base()) {
        cast_to<EnumType>(new_node)->set_base(cast_to<DataType>(get_base()->clone()));
    }
    cast_to<EnumType>(new_node)->set_items(EnumItem::clone_list(get_items()));
    cast_to<EnumType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr EnumType::alloc_same(void) const
{
    Ptr p(new EnumType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const EnumType &p)
{
    os << "EnumType: {";
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

std::ostream &operator<<(std::ostream &os, const EnumType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "EnumType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse