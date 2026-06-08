// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/alwayscomb.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

AlwaysComb::AlwaysComb(const std::string &filename, uint32_t line) : Always(filename, line)
{
    set_node_type(NodeType::AlwaysComb);
    set_node_categories({NodeType::Always, NodeType::Node});
}

AlwaysComb::AlwaysComb(const Senslist::Ptr senslist, const Node::Ptr statement,
                       const std::string &filename, uint32_t line)
    : Always(senslist, statement, filename, line)
{
    set_node_type(NodeType::AlwaysComb);
    set_node_categories({NodeType::Always, NodeType::Node});
}

AlwaysComb &AlwaysComb::operator=(const AlwaysComb &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &AlwaysComb::operator=(const Node &rhs)
{
    const AlwaysComb &rhs_cast = static_cast<const AlwaysComb &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool AlwaysComb::operator==(const AlwaysComb &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool AlwaysComb::operator==(const Node &rhs) const
{
    const AlwaysComb &rhs_cast = static_cast<const AlwaysComb &>(rhs);
    return operator==(rhs_cast);
}

bool AlwaysComb::operator!=(const AlwaysComb &rhs) const { return !(operator==(rhs)); }

bool AlwaysComb::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool AlwaysComb::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool AlwaysComb::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_senslist()) {
        if(get_senslist() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "AlwaysComb::replace matches multiple times (Senslist::senslist)";
            }
            set_senslist(cast_to<Senslist>(new_node));
            found = true;
        }
    }
    if(get_statement()) {
        if(get_statement() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "AlwaysComb::replace matches multiple times (Node::statement)";
            }
            set_statement(new_node);
            found = true;
        }
    }
    return found;
}

bool AlwaysComb::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

AlwaysComb::ListPtr AlwaysComb::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<AlwaysComb>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr AlwaysComb::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_senslist()) {
        list->push_back(std::static_pointer_cast<Node>(get_senslist()));
    }
    if(get_statement()) {
        list->push_back(std::static_pointer_cast<Node>(get_statement()));
    }
    return list;
}

void AlwaysComb::clone_children(Node::Ptr new_node) const
{
    if(get_senslist()) {
        cast_to<AlwaysComb>(new_node)->set_senslist(cast_to<Senslist>(get_senslist()->clone()));
    }
    if(get_statement()) {
        cast_to<AlwaysComb>(new_node)->set_statement(get_statement()->clone());
    }
}

Node::Ptr AlwaysComb::alloc_same(void) const
{
    Ptr p(new AlwaysComb);
    return p;
}

std::ostream &operator<<(std::ostream &os, const AlwaysComb &p)
{
    os << "AlwaysComb: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const AlwaysComb::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "AlwaysComb: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse