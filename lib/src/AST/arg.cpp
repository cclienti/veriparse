// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/arg.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Arg::Arg(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::Arg);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Arg::Arg(const Dimension::ListPtr unpacked_dims, const Node::Ptr default_value,
         const DataType::Ptr type, const bool &is_var, const DirectionEnum &direction,
         const std::string &name, const std::string &filename, uint32_t line)
    : Declaration(type, name, filename, line), m_unpacked_dims(unpacked_dims),
      m_default_value(default_value), m_is_var(is_var), m_direction(direction)
{
    set_node_type(NodeType::Arg);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Arg &Arg::operator=(const Arg &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_var(rhs.get_is_var());
    set_direction(rhs.get_direction());
    set_name(rhs.get_name());
    return *this;
}

Node &Arg::operator=(const Node &rhs)
{
    const Arg &rhs_cast = static_cast<const Arg &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Arg::operator==(const Arg &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_var() != rhs.get_is_var()) {
        return false;
    }
    if(get_direction() != rhs.get_direction()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Arg::operator==(const Node &rhs) const
{
    const Arg &rhs_cast = static_cast<const Arg &>(rhs);
    return operator==(rhs_cast);
}

bool Arg::operator!=(const Arg &rhs) const { return !(operator==(rhs)); }

bool Arg::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Arg::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Arg::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "Arg::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Arg::replace "
                            << "of children list(Dimension)::unpacked_dims";
            }
        }
        if(new_list->size() != 0) {
            set_unpacked_dims(new_list);
        } else {
            set_unpacked_dims(nullptr);
        }
    }
    if(get_default_value()) {
        if(get_default_value() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Arg::replace matches multiple times (Node::default_value)";
            }
            set_default_value(new_node);
            found = true;
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Arg::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Arg::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "Arg::replace matches multiple times "
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
                            << "found an empty node during Arg::replace "
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

Arg::ListPtr Arg::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Arg>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Arg::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_unpacked_dims()) {
        for(const Dimension::Ptr &node : *get_unpacked_dims()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_default_value()) {
        list->push_back(std::static_pointer_cast<Node>(get_default_value()));
    }
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void Arg::clone_children(Node::Ptr new_node) const
{
    cast_to<Arg>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_default_value()) {
        cast_to<Arg>(new_node)->set_default_value(get_default_value()->clone());
    }
    if(get_type()) {
        cast_to<Arg>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Arg::alloc_same(void) const
{
    Ptr p(new Arg);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Arg &p)
{
    os << "Arg: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_var: " << p.get_is_var() << ", ";

    os << "direction: " << p.get_direction() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Arg::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Arg: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Arg::DirectionEnum p)
{
    switch(p) {
    case Arg::DirectionEnum::INPUT:
        os << "INPUT";
        break;
    case Arg::DirectionEnum::OUTPUT:
        os << "OUTPUT";
        break;
    case Arg::DirectionEnum::INOUT:
        os << "INOUT";
        break;
    case Arg::DirectionEnum::REF:
        os << "REF";
        break;
    case Arg::DirectionEnum::CONST_REF:
        os << "CONST_REF";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse