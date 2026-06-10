// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/var.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Var::Var(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::Var);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Var::Var(const Dimension::ListPtr unpacked_dims, const Rvalue::Ptr init, const DataType::Ptr type,
         const bool &is_var, const bool &is_const, const LifetimeEnum &lifetime,
         const std::string &name, const std::string &filename, uint32_t line)
    : Declaration(type, name, filename, line), m_unpacked_dims(unpacked_dims), m_init(init),
      m_is_var(is_var), m_is_const(is_const), m_lifetime(lifetime)
{
    set_node_type(NodeType::Var);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Var &Var::operator=(const Var &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_var(rhs.get_is_var());
    set_is_const(rhs.get_is_const());
    set_lifetime(rhs.get_lifetime());
    set_name(rhs.get_name());
    return *this;
}

Node &Var::operator=(const Node &rhs)
{
    const Var &rhs_cast = static_cast<const Var &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Var::operator==(const Var &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_var() != rhs.get_is_var()) {
        return false;
    }
    if(get_is_const() != rhs.get_is_const()) {
        return false;
    }
    if(get_lifetime() != rhs.get_lifetime()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Var::operator==(const Node &rhs) const
{
    const Var &rhs_cast = static_cast<const Var &>(rhs);
    return operator==(rhs_cast);
}

bool Var::operator!=(const Var &rhs) const { return !(operator==(rhs)); }

bool Var::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Var::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Var::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "Var::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Var::replace "
                            << "of children list(Dimension)::unpacked_dims";
            }
        }
        if(new_list->size() != 0) {
            set_unpacked_dims(new_list);
        } else {
            set_unpacked_dims(nullptr);
        }
    }
    if(get_init()) {
        if(get_init() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Var::replace matches multiple times (Rvalue::init)";
            }
            set_init(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Var::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Var::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "Var::replace matches multiple times "
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
                            << "found an empty node during Var::replace "
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

Var::ListPtr Var::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Var>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Var::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_unpacked_dims()) {
        for(const Dimension::Ptr &node : *get_unpacked_dims()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_init()) {
        list->push_back(std::static_pointer_cast<Node>(get_init()));
    }
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void Var::clone_children(Node::Ptr new_node) const
{
    cast_to<Var>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_init()) {
        cast_to<Var>(new_node)->set_init(cast_to<Rvalue>(get_init()->clone()));
    }
    if(get_type()) {
        cast_to<Var>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Var::alloc_same(void) const
{
    Ptr p(new Var);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Var &p)
{
    os << "Var: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_var: " << p.get_is_var() << ", ";

    os << "is_const: " << p.get_is_const() << ", ";

    os << "lifetime: " << p.get_lifetime() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Var::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Var: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Var::LifetimeEnum p)
{
    switch(p) {
    case Var::LifetimeEnum::NONE:
        os << "NONE";
        break;
    case Var::LifetimeEnum::AUTOMATIC:
        os << "AUTOMATIC";
        break;
    case Var::LifetimeEnum::STATIC:
        os << "STATIC";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse