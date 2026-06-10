// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/typecast.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

TypeCast::TypeCast(const std::string &filename, uint32_t line) : Cast(filename, line)
{
    set_node_type(NodeType::TypeCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

TypeCast::TypeCast(const DataType::Ptr target, const Node::Ptr expr, const std::string &filename,
                   uint32_t line)
    : Cast(expr, filename, line), m_target(target)
{
    set_node_type(NodeType::TypeCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

TypeCast &TypeCast::operator=(const TypeCast &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &TypeCast::operator=(const Node &rhs)
{
    const TypeCast &rhs_cast = static_cast<const TypeCast &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool TypeCast::operator==(const TypeCast &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool TypeCast::operator==(const Node &rhs) const
{
    const TypeCast &rhs_cast = static_cast<const TypeCast &>(rhs);
    return operator==(rhs_cast);
}

bool TypeCast::operator!=(const TypeCast &rhs) const { return !(operator==(rhs)); }

bool TypeCast::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool TypeCast::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool TypeCast::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_target()) {
        if(get_target() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "TypeCast::replace matches multiple times (DataType::target)";
            }
            set_target(cast_to<DataType>(new_node));
            found = true;
        }
    }
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "TypeCast::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool TypeCast::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

TypeCast::ListPtr TypeCast::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<TypeCast>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr TypeCast::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_target()) {
        list->push_back(std::static_pointer_cast<Node>(get_target()));
    }
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void TypeCast::clone_children(Node::Ptr new_node) const
{
    if(get_target()) {
        cast_to<TypeCast>(new_node)->set_target(cast_to<DataType>(get_target()->clone()));
    }
    if(get_expr()) {
        cast_to<TypeCast>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr TypeCast::alloc_same(void) const
{
    Ptr p(new TypeCast);
    return p;
}

std::ostream &operator<<(std::ostream &os, const TypeCast &p)
{
    os << "TypeCast: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TypeCast::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "TypeCast: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse