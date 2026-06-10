// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/unsizeddim.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

UnsizedDim::UnsizedDim(const std::string &filename, uint32_t line) : Dimension(filename, line)
{
    set_node_type(NodeType::UnsizedDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

UnsizedDim &UnsizedDim::operator=(const UnsizedDim &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &UnsizedDim::operator=(const Node &rhs)
{
    const UnsizedDim &rhs_cast = static_cast<const UnsizedDim &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool UnsizedDim::operator==(const UnsizedDim &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool UnsizedDim::operator==(const Node &rhs) const
{
    const UnsizedDim &rhs_cast = static_cast<const UnsizedDim &>(rhs);
    return operator==(rhs_cast);
}

bool UnsizedDim::operator!=(const UnsizedDim &rhs) const { return !(operator==(rhs)); }

bool UnsizedDim::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool UnsizedDim::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool UnsizedDim::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool UnsizedDim::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

UnsizedDim::ListPtr UnsizedDim::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<UnsizedDim>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr UnsizedDim::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void UnsizedDim::clone_children(Node::Ptr new_node) const {}

Node::Ptr UnsizedDim::alloc_same(void) const
{
    Ptr p(new UnsizedDim);
    return p;
}

std::ostream &operator<<(std::ostream &os, const UnsizedDim &p)
{
    os << "UnsizedDim: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const UnsizedDim::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "UnsizedDim: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse