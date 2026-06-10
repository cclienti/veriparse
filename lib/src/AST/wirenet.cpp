// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/wirenet.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

WireNet::WireNet(const std::string &filename, uint32_t line) : Net(filename, line)
{
    set_node_type(NodeType::WireNet);
    set_node_categories({NodeType::Net, NodeType::Declaration, NodeType::Node});
}

WireNet::WireNet(const Dimension::ListPtr unpacked_dims, const Rvalue::Ptr cont_assign,
                 const Strength::Ptr strength, const DelayStatement::Ptr ldelay,
                 const DelayStatement::Ptr rdelay, const DataType::Ptr type,
                 const bool &is_vectored, const bool &is_scalared, const std::string &name,
                 const std::string &filename, uint32_t line)
    : Net(unpacked_dims, cont_assign, strength, ldelay, rdelay, type, is_vectored, is_scalared,
          name, filename, line)
{
    set_node_type(NodeType::WireNet);
    set_node_categories({NodeType::Net, NodeType::Declaration, NodeType::Node});
}

WireNet &WireNet::operator=(const WireNet &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_vectored(rhs.get_is_vectored());
    set_is_scalared(rhs.get_is_scalared());
    set_name(rhs.get_name());
    return *this;
}

Node &WireNet::operator=(const Node &rhs)
{
    const WireNet &rhs_cast = static_cast<const WireNet &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool WireNet::operator==(const WireNet &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_vectored() != rhs.get_is_vectored()) {
        return false;
    }
    if(get_is_scalared() != rhs.get_is_scalared()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool WireNet::operator==(const Node &rhs) const
{
    const WireNet &rhs_cast = static_cast<const WireNet &>(rhs);
    return operator==(rhs_cast);
}

bool WireNet::operator!=(const WireNet &rhs) const { return !(operator==(rhs)); }

bool WireNet::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool WireNet::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool WireNet::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "WireNet::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during WireNet::replace "
                            << "of children list(Dimension)::unpacked_dims";
            }
        }
        if(new_list->size() != 0) {
            set_unpacked_dims(new_list);
        } else {
            set_unpacked_dims(nullptr);
        }
    }
    if(get_cont_assign()) {
        if(get_cont_assign() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "WireNet::replace matches multiple times (Rvalue::cont_assign)";
            }
            set_cont_assign(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    if(get_strength()) {
        if(get_strength() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "WireNet::replace matches multiple times (Strength::strength)";
            }
            set_strength(cast_to<Strength>(new_node));
            found = true;
        }
    }
    if(get_ldelay()) {
        if(get_ldelay() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "WireNet::replace matches multiple times (DelayStatement::ldelay)";
            }
            set_ldelay(cast_to<DelayStatement>(new_node));
            found = true;
        }
    }
    if(get_rdelay()) {
        if(get_rdelay() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "WireNet::replace matches multiple times (DelayStatement::rdelay)";
            }
            set_rdelay(cast_to<DelayStatement>(new_node));
            found = true;
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "WireNet::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool WireNet::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "WireNet::replace matches multiple times "
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
                            << "found an empty node during WireNet::replace "
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

WireNet::ListPtr WireNet::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<WireNet>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr WireNet::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_unpacked_dims()) {
        for(const Dimension::Ptr &node : *get_unpacked_dims()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_cont_assign()) {
        list->push_back(std::static_pointer_cast<Node>(get_cont_assign()));
    }
    if(get_strength()) {
        list->push_back(std::static_pointer_cast<Node>(get_strength()));
    }
    if(get_ldelay()) {
        list->push_back(std::static_pointer_cast<Node>(get_ldelay()));
    }
    if(get_rdelay()) {
        list->push_back(std::static_pointer_cast<Node>(get_rdelay()));
    }
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void WireNet::clone_children(Node::Ptr new_node) const
{
    cast_to<WireNet>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_cont_assign()) {
        cast_to<WireNet>(new_node)->set_cont_assign(cast_to<Rvalue>(get_cont_assign()->clone()));
    }
    if(get_strength()) {
        cast_to<WireNet>(new_node)->set_strength(cast_to<Strength>(get_strength()->clone()));
    }
    if(get_ldelay()) {
        cast_to<WireNet>(new_node)->set_ldelay(cast_to<DelayStatement>(get_ldelay()->clone()));
    }
    if(get_rdelay()) {
        cast_to<WireNet>(new_node)->set_rdelay(cast_to<DelayStatement>(get_rdelay()->clone()));
    }
    if(get_type()) {
        cast_to<WireNet>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr WireNet::alloc_same(void) const
{
    Ptr p(new WireNet);
    return p;
}

std::ostream &operator<<(std::ostream &os, const WireNet &p)
{
    os << "WireNet: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_vectored: " << p.get_is_vectored() << ", ";

    os << "is_scalared: " << p.get_is_scalared() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const WireNet::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "WireNet: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse