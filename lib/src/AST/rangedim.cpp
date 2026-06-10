// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/rangedim.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

RangeDim::RangeDim(const std::string &filename, uint32_t line) : Dimension(filename, line)
{
    set_node_type(NodeType::RangeDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

RangeDim::RangeDim(const Node::Ptr left, const Node::Ptr right, const std::string &filename,
                   uint32_t line)
    : Dimension(filename, line), m_left(left), m_right(right)
{
    set_node_type(NodeType::RangeDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

RangeDim &RangeDim::operator=(const RangeDim &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &RangeDim::operator=(const Node &rhs)
{
    const RangeDim &rhs_cast = static_cast<const RangeDim &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool RangeDim::operator==(const RangeDim &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool RangeDim::operator==(const Node &rhs) const
{
    const RangeDim &rhs_cast = static_cast<const RangeDim &>(rhs);
    return operator==(rhs_cast);
}

bool RangeDim::operator!=(const RangeDim &rhs) const { return !(operator==(rhs)); }

bool RangeDim::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool RangeDim::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool RangeDim::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_left()) {
        if(get_left() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "RangeDim::replace matches multiple times (Node::left)";
            }
            set_left(new_node);
            found = true;
        }
    }
    if(get_right()) {
        if(get_right() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "RangeDim::replace matches multiple times (Node::right)";
            }
            set_right(new_node);
            found = true;
        }
    }
    return found;
}

bool RangeDim::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

RangeDim::ListPtr RangeDim::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<RangeDim>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr RangeDim::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_left()) {
        list->push_back(std::static_pointer_cast<Node>(get_left()));
    }
    if(get_right()) {
        list->push_back(std::static_pointer_cast<Node>(get_right()));
    }
    return list;
}

void RangeDim::clone_children(Node::Ptr new_node) const
{
    if(get_left()) {
        cast_to<RangeDim>(new_node)->set_left(get_left()->clone());
    }
    if(get_right()) {
        cast_to<RangeDim>(new_node)->set_right(get_right()->clone());
    }
}

Node::Ptr RangeDim::alloc_same(void) const
{
    Ptr p(new RangeDim);
    return p;
}

std::ostream &operator<<(std::ostream &os, const RangeDim &p)
{
    os << "RangeDim: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const RangeDim::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "RangeDim: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse