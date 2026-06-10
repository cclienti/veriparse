// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/sizedim.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

SizeDim::SizeDim(const std::string &filename, uint32_t line) : Dimension(filename, line)
{
    set_node_type(NodeType::SizeDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

SizeDim::SizeDim(const Node::Ptr size, const std::string &filename, uint32_t line)
    : Dimension(filename, line), m_size(size)
{
    set_node_type(NodeType::SizeDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

SizeDim &SizeDim::operator=(const SizeDim &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &SizeDim::operator=(const Node &rhs)
{
    const SizeDim &rhs_cast = static_cast<const SizeDim &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool SizeDim::operator==(const SizeDim &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool SizeDim::operator==(const Node &rhs) const
{
    const SizeDim &rhs_cast = static_cast<const SizeDim &>(rhs);
    return operator==(rhs_cast);
}

bool SizeDim::operator!=(const SizeDim &rhs) const { return !(operator==(rhs)); }

bool SizeDim::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool SizeDim::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool SizeDim::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_size()) {
        if(get_size() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "SizeDim::replace matches multiple times (Node::size)";
            }
            set_size(new_node);
            found = true;
        }
    }
    return found;
}

bool SizeDim::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

SizeDim::ListPtr SizeDim::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<SizeDim>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr SizeDim::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_size()) {
        list->push_back(std::static_pointer_cast<Node>(get_size()));
    }
    return list;
}

void SizeDim::clone_children(Node::Ptr new_node) const
{
    if(get_size()) {
        cast_to<SizeDim>(new_node)->set_size(get_size()->clone());
    }
}

Node::Ptr SizeDim::alloc_same(void) const
{
    Ptr p(new SizeDim);
    return p;
}

std::ostream &operator<<(std::ostream &os, const SizeDim &p)
{
    os << "SizeDim: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SizeDim::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "SizeDim: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse