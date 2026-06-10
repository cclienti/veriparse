// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/namedtype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

NamedType::NamedType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::NamedType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

NamedType::NamedType(const ScopeName::ListPtr scope, const Dimension::ListPtr packed_dims,
                     const std::string &name, const DataType::SigningEnum &signing,
                     const std::string &filename, uint32_t line)
    : DataType(packed_dims, signing, filename, line), m_scope(scope), m_name(name)
{
    set_node_type(NodeType::NamedType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

NamedType &NamedType::operator=(const NamedType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_signing(rhs.get_signing());
    return *this;
}

Node &NamedType::operator=(const Node &rhs)
{
    const NamedType &rhs_cast = static_cast<const NamedType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool NamedType::operator==(const NamedType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool NamedType::operator==(const Node &rhs) const
{
    const NamedType &rhs_cast = static_cast<const NamedType &>(rhs);
    return operator==(rhs_cast);
}

bool NamedType::operator!=(const NamedType &rhs) const { return !(operator==(rhs)); }

bool NamedType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool NamedType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool NamedType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_scope()) {
        ScopeName::ListPtr new_list = std::make_shared<ScopeName::List>();
        for(const ScopeName::Ptr &lnode : *get_scope()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "NamedType::replace matches multiple times (list(ScopeName)::scope)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ScopeName>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during NamedType::replace "
                            << "of children list(ScopeName)::scope";
            }
        }
        if(new_list->size() != 0) {
            set_scope(new_list);
        } else {
            set_scope(nullptr);
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
                                    << "NamedType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during NamedType::replace "
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

bool NamedType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_scope()) {
        ScopeName::ListPtr new_list = std::make_shared<ScopeName::List>();
        for(const ScopeName::Ptr &lnode : *get_scope()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "NamedType::replace matches multiple times (list(ScopeName)::scope)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<ScopeName>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during NamedType::replace "
                            << "of children list(ScopeName)::scope";
            }
        }
        if(new_list->size() != 0) {
            set_scope(new_list);
        } else {
            set_scope(nullptr);
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
                                    << "NamedType::replace matches multiple times "
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
                            << "found an empty node during NamedType::replace "
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

NamedType::ListPtr NamedType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<NamedType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr NamedType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_scope()) {
        for(const ScopeName::Ptr &node : *get_scope()) {
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

void NamedType::clone_children(Node::Ptr new_node) const
{
    cast_to<NamedType>(new_node)->set_scope(ScopeName::clone_list(get_scope()));
    cast_to<NamedType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr NamedType::alloc_same(void) const
{
    Ptr p(new NamedType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const NamedType &p)
{
    os << "NamedType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const NamedType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "NamedType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse