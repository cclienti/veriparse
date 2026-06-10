// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/typedef.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Typedef::Typedef(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::Typedef);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Typedef::Typedef(const Dimension::ListPtr unpacked_dims, const DataType::Ptr type,
                 const Fwd_kindEnum &fwd_kind, const std::string &name, const std::string &filename,
                 uint32_t line)
    : Declaration(type, name, filename, line), m_unpacked_dims(unpacked_dims), m_fwd_kind(fwd_kind)
{
    set_node_type(NodeType::Typedef);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Typedef &Typedef::operator=(const Typedef &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_fwd_kind(rhs.get_fwd_kind());
    set_name(rhs.get_name());
    return *this;
}

Node &Typedef::operator=(const Node &rhs)
{
    const Typedef &rhs_cast = static_cast<const Typedef &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Typedef::operator==(const Typedef &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_fwd_kind() != rhs.get_fwd_kind()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Typedef::operator==(const Node &rhs) const
{
    const Typedef &rhs_cast = static_cast<const Typedef &>(rhs);
    return operator==(rhs_cast);
}

bool Typedef::operator!=(const Typedef &rhs) const { return !(operator==(rhs)); }

bool Typedef::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Typedef::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Typedef::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_unpacked_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_unpacked_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Typedef::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Typedef::replace "
                            << "of children list(Dimension)::unpacked_dims";
            }
        }
        if(new_list->size() != 0) {
            set_unpacked_dims(new_list);
        } else {
            set_unpacked_dims(nullptr);
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Typedef::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Typedef::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_unpacked_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_unpacked_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Typedef::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
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
                            << "found an empty node during Typedef::replace "
                            << "of children list(Dimension)::unpacked_dims";
            }
        }
        if(new_list->size() != 0) {
            set_unpacked_dims(new_list);
        } else {
            set_unpacked_dims(nullptr);
        }
    }
    return found;
}

Typedef::ListPtr Typedef::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Typedef>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Typedef::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_unpacked_dims()) {
        for(const Dimension::Ptr &node : *get_unpacked_dims()) {
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

void Typedef::clone_children(Node::Ptr new_node) const
{
    cast_to<Typedef>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_type()) {
        cast_to<Typedef>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Typedef::alloc_same(void) const
{
    Ptr p(new Typedef);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Typedef &p)
{
    os << "Typedef: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "fwd_kind: " << p.get_fwd_kind() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Typedef::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Typedef: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Typedef::Fwd_kindEnum p)
{
    switch(p) {
    case Typedef::Fwd_kindEnum::NONE:
        os << "NONE";
        break;
    case Typedef::Fwd_kindEnum::ENUM:
        os << "ENUM";
        break;
    case Typedef::Fwd_kindEnum::STRUCT:
        os << "STRUCT";
        break;
    case Typedef::Fwd_kindEnum::UNION:
        os << "UNION";
        break;
    case Typedef::Fwd_kindEnum::CLASS:
        os << "CLASS";
        break;
    case Typedef::Fwd_kindEnum::INTERFACE_CLASS:
        os << "INTERFACE_CLASS";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse