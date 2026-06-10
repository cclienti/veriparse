// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/param.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Param::Param(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::Param);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Param::Param(const Node::Ptr value, const Dimension::ListPtr unpacked_dims,
             const DataType::Ptr type, const bool &is_local, const std::string &name,
             const std::string &filename, uint32_t line)
    : Declaration(type, name, filename, line), m_value(value), m_unpacked_dims(unpacked_dims),
      m_is_local(is_local)
{
    set_node_type(NodeType::Param);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Param &Param::operator=(const Param &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_local(rhs.get_is_local());
    set_name(rhs.get_name());
    return *this;
}

Node &Param::operator=(const Node &rhs)
{
    const Param &rhs_cast = static_cast<const Param &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Param::operator==(const Param &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_local() != rhs.get_is_local()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Param::operator==(const Node &rhs) const
{
    const Param &rhs_cast = static_cast<const Param &>(rhs);
    return operator==(rhs_cast);
}

bool Param::operator!=(const Param &rhs) const { return !(operator==(rhs)); }

bool Param::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Param::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Param::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_value()) {
        if(get_value() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Param::replace matches multiple times (Node::value)";
            }
            set_value(new_node);
            found = true;
        }
    }
    if(get_unpacked_dims()) {
        Dimension::ListPtr new_list = std::make_shared<Dimension::List>();
        for(const Dimension::Ptr &lnode : *get_unpacked_dims()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Param::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Param::replace "
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
                            << "Param::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Param::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "Param::replace matches multiple times "
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
                            << "found an empty node during Param::replace "
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

Param::ListPtr Param::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Param>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Param::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_value()) {
        list->push_back(std::static_pointer_cast<Node>(get_value()));
    }
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

void Param::clone_children(Node::Ptr new_node) const
{
    if(get_value()) {
        cast_to<Param>(new_node)->set_value(get_value()->clone());
    }
    cast_to<Param>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_type()) {
        cast_to<Param>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Param::alloc_same(void) const
{
    Ptr p(new Param);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Param &p)
{
    os << "Param: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_local: " << p.get_is_local() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Param::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Param: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse