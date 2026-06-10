// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/queuedim.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

QueueDim::QueueDim(const std::string &filename, uint32_t line) : Dimension(filename, line)
{
    set_node_type(NodeType::QueueDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

QueueDim::QueueDim(const Node::Ptr bound, const std::string &filename, uint32_t line)
    : Dimension(filename, line), m_bound(bound)
{
    set_node_type(NodeType::QueueDim);
    set_node_categories({NodeType::Dimension, NodeType::Node});
}

QueueDim &QueueDim::operator=(const QueueDim &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &QueueDim::operator=(const Node &rhs)
{
    const QueueDim &rhs_cast = static_cast<const QueueDim &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool QueueDim::operator==(const QueueDim &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool QueueDim::operator==(const Node &rhs) const
{
    const QueueDim &rhs_cast = static_cast<const QueueDim &>(rhs);
    return operator==(rhs_cast);
}

bool QueueDim::operator!=(const QueueDim &rhs) const { return !(operator==(rhs)); }

bool QueueDim::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool QueueDim::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool QueueDim::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_bound()) {
        if(get_bound() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "QueueDim::replace matches multiple times (Node::bound)";
            }
            set_bound(new_node);
            found = true;
        }
    }
    return found;
}

bool QueueDim::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

QueueDim::ListPtr QueueDim::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<QueueDim>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr QueueDim::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_bound()) {
        list->push_back(std::static_pointer_cast<Node>(get_bound()));
    }
    return list;
}

void QueueDim::clone_children(Node::Ptr new_node) const
{
    if(get_bound()) {
        cast_to<QueueDim>(new_node)->set_bound(get_bound()->clone());
    }
}

Node::Ptr QueueDim::alloc_same(void) const
{
    Ptr p(new QueueDim);
    return p;
}

std::ostream &operator<<(std::ostream &os, const QueueDim &p)
{
    os << "QueueDim: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const QueueDim::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "QueueDim: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse