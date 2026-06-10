// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/sizecast.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

SizeCast::SizeCast(const std::string &filename, uint32_t line) : Cast(filename, line)
{
    set_node_type(NodeType::SizeCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

SizeCast::SizeCast(const Node::Ptr size, const Node::Ptr expr, const std::string &filename,
                   uint32_t line)
    : Cast(expr, filename, line), m_size(size)
{
    set_node_type(NodeType::SizeCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

SizeCast &SizeCast::operator=(const SizeCast &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &SizeCast::operator=(const Node &rhs)
{
    const SizeCast &rhs_cast = static_cast<const SizeCast &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool SizeCast::operator==(const SizeCast &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool SizeCast::operator==(const Node &rhs) const
{
    const SizeCast &rhs_cast = static_cast<const SizeCast &>(rhs);
    return operator==(rhs_cast);
}

bool SizeCast::operator!=(const SizeCast &rhs) const { return !(operator==(rhs)); }

bool SizeCast::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool SizeCast::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool SizeCast::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_size()) {
        if(get_size() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "SizeCast::replace matches multiple times (Node::size)";
            }
            set_size(new_node);
            found = true;
        }
    }
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "SizeCast::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool SizeCast::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

SizeCast::ListPtr SizeCast::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<SizeCast>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr SizeCast::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_size()) {
        list->push_back(std::static_pointer_cast<Node>(get_size()));
    }
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void SizeCast::clone_children(Node::Ptr new_node) const
{
    if(get_size()) {
        cast_to<SizeCast>(new_node)->set_size(get_size()->clone());
    }
    if(get_expr()) {
        cast_to<SizeCast>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr SizeCast::alloc_same(void) const
{
    Ptr p(new SizeCast);
    return p;
}

std::ostream &operator<<(std::ostream &os, const SizeCast &p)
{
    os << "SizeCast: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SizeCast::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "SizeCast: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse