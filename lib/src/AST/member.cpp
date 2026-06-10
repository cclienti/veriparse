// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/member.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Member::Member(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::Member);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Member::Member(const Dimension::ListPtr unpacked_dims, const Rvalue::Ptr init,
               const DataType::Ptr type, const std::string &name, const std::string &filename,
               uint32_t line)
    : Declaration(type, name, filename, line), m_unpacked_dims(unpacked_dims), m_init(init)
{
    set_node_type(NodeType::Member);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

Member &Member::operator=(const Member &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Member::operator=(const Node &rhs)
{
    const Member &rhs_cast = static_cast<const Member &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Member::operator==(const Member &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Member::operator==(const Node &rhs) const
{
    const Member &rhs_cast = static_cast<const Member &>(rhs);
    return operator==(rhs_cast);
}

bool Member::operator!=(const Member &rhs) const { return !(operator==(rhs)); }

bool Member::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Member::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Member::replace(Node::Ptr node, Node::Ptr new_node)
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
                                    << "Member::replace matches multiple times "
                                       "(list(Dimension)::unpacked_dims)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Dimension>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Member::replace "
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
                            << "Member::replace matches multiple times (Rvalue::init)";
            }
            set_init(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Member::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Member::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                                    << "Member::replace matches multiple times "
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
                            << "found an empty node during Member::replace "
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

Member::ListPtr Member::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Member>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Member::get_children(void) const
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

void Member::clone_children(Node::Ptr new_node) const
{
    cast_to<Member>(new_node)->set_unpacked_dims(Dimension::clone_list(get_unpacked_dims()));
    if(get_init()) {
        cast_to<Member>(new_node)->set_init(cast_to<Rvalue>(get_init()->clone()));
    }
    if(get_type()) {
        cast_to<Member>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Member::alloc_same(void) const
{
    Ptr p(new Member);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Member &p)
{
    os << "Member: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Member::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Member: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse