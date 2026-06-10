// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/assocdim.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

AssocDim::AssocDim(const std::string &filename, uint32_t line) : Dimension(filename, line)
{
    set_node_type(NodeType::AssocDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

AssocDim::AssocDim(const DataType::Ptr index_type, const std::string &filename, uint32_t line)
    : Dimension(filename, line), m_index_type(index_type)
{
    set_node_type(NodeType::AssocDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

AssocDim &AssocDim::operator=(const AssocDim &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &AssocDim::operator=(const Node &rhs)
{
    const AssocDim &rhs_cast = static_cast<const AssocDim &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool AssocDim::operator==(const AssocDim &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool AssocDim::operator==(const Node &rhs) const
{
    const AssocDim &rhs_cast = static_cast<const AssocDim &>(rhs);
    return operator==(rhs_cast);
}

bool AssocDim::operator!=(const AssocDim &rhs) const { return !(operator==(rhs)); }

bool AssocDim::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool AssocDim::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool AssocDim::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_index_type()) {
        if(get_index_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "AssocDim::replace matches multiple times (DataType::index_type)";
            }
            set_index_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool AssocDim::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

AssocDim::ListPtr AssocDim::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<AssocDim>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr AssocDim::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_index_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_index_type()));
    }
    return list;
}

void AssocDim::clone_children(Node::Ptr new_node) const
{
    if(get_index_type()) {
        cast_to<AssocDim>(new_node)->set_index_type(cast_to<DataType>(get_index_type()->clone()));
    }
}

Node::Ptr AssocDim::alloc_same(void) const
{
    Ptr p(new AssocDim);
    return p;
}

std::ostream &operator<<(std::ostream &os, const AssocDim &p)
{
    os << "AssocDim: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const AssocDim::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "AssocDim: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse