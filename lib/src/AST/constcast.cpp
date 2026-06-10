// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/constcast.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ConstCast::ConstCast(const std::string &filename, uint32_t line) : Cast(filename, line)
{
    set_node_type(NodeType::ConstCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

ConstCast::ConstCast(const Node::Ptr expr, const std::string &filename, uint32_t line)
    : Cast(expr, filename, line)
{
    set_node_type(NodeType::ConstCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

ConstCast &ConstCast::operator=(const ConstCast &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &ConstCast::operator=(const Node &rhs)
{
    const ConstCast &rhs_cast = static_cast<const ConstCast &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ConstCast::operator==(const ConstCast &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool ConstCast::operator==(const Node &rhs) const
{
    const ConstCast &rhs_cast = static_cast<const ConstCast &>(rhs);
    return operator==(rhs_cast);
}

bool ConstCast::operator!=(const ConstCast &rhs) const { return !(operator==(rhs)); }

bool ConstCast::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ConstCast::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ConstCast::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "ConstCast::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool ConstCast::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

ConstCast::ListPtr ConstCast::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ConstCast>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ConstCast::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void ConstCast::clone_children(Node::Ptr new_node) const
{
    if(get_expr()) {
        cast_to<ConstCast>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr ConstCast::alloc_same(void) const
{
    Ptr p(new ConstCast);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ConstCast &p)
{
    os << "ConstCast: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ConstCast::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ConstCast: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse