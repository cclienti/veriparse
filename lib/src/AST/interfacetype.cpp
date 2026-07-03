// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/interfacetype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

InterfaceType::InterfaceType(const std::string &filename, uint32_t line) : DataType(filename, line)
{
    set_node_type(NodeType::InterfaceType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

InterfaceType::InterfaceType(const ParamArg::ListPtr params, const Dimension::ListPtr packed_dims,
                             const std::string &name, const std::string &modport,
                             const bool &is_virtual, const DataType::SigningEnum &signing,
                             const std::string &filename, uint32_t line)
    : DataType(packed_dims, signing, filename, line), m_params(params), m_name(name),
      m_modport(modport), m_is_virtual(is_virtual)
{
    set_node_type(NodeType::InterfaceType);
    set_node_categories({NodeType::DataType, NodeType::Node});
}

InterfaceType &InterfaceType::operator=(const InterfaceType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_modport(rhs.get_modport());
    set_is_virtual(rhs.get_is_virtual());
    set_signing(rhs.get_signing());
    return *this;
}

Node &InterfaceType::operator=(const Node &rhs)
{
    const InterfaceType &rhs_cast = static_cast<const InterfaceType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool InterfaceType::operator==(const InterfaceType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    if(get_modport() != rhs.get_modport()) {
        return false;
    }
    if(get_is_virtual() != rhs.get_is_virtual()) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool InterfaceType::operator==(const Node &rhs) const
{
    const InterfaceType &rhs_cast = static_cast<const InterfaceType &>(rhs);
    return operator==(rhs_cast);
}

bool InterfaceType::operator!=(const InterfaceType &rhs) const { return !(operator==(rhs)); }

bool InterfaceType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool InterfaceType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool InterfaceType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_params()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceType::replace matches multiple times "
                                       "(list(ParamArg)::params)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ParamArg>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceType::replace "
                            << "of children list(ParamArg)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
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
                                    << "InterfaceType::replace matches multiple times "
                                       "(list(Dimension)::packed_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceType::replace "
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

bool InterfaceType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_params()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceType::replace matches multiple times "
                                       "(list(ParamArg)::params)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<ParamArg>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceType::replace "
                            << "of children list(ParamArg)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
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
                                    << "InterfaceType::replace matches multiple times "
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
                            << "found an empty node during InterfaceType::replace "
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

InterfaceType::ListPtr InterfaceType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<InterfaceType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr InterfaceType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_params()) {
        for(const ParamArg::Ptr &node : *get_params()) {
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

void InterfaceType::clone_children(Node::Ptr new_node) const
{
    cast_to<InterfaceType>(new_node)->set_params(ParamArg::clone_list(get_params()));
    cast_to<InterfaceType>(new_node)->set_packed_dims(Dimension::clone_list(get_packed_dims()));
}

Node::Ptr InterfaceType::alloc_same(void) const
{
    Ptr p(new InterfaceType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const InterfaceType &p)
{
    os << "InterfaceType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "modport: " << p.get_modport() << ", ";

    os << "is_virtual: " << p.get_is_virtual() << ", ";

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const InterfaceType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "InterfaceType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse