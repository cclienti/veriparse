// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/signingcast.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

SigningCast::SigningCast(const std::string &filename, uint32_t line) : Cast(filename, line)
{
    set_node_type(NodeType::SigningCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

SigningCast::SigningCast(const Node::Ptr expr, const SigningEnum &signing,
                         const std::string &filename, uint32_t line)
    : Cast(expr, filename, line), m_signing(signing)
{
    set_node_type(NodeType::SigningCast);
    set_node_categories({NodeType::Cast, NodeType::Node});
}

SigningCast &SigningCast::operator=(const SigningCast &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_signing(rhs.get_signing());
    return *this;
}

Node &SigningCast::operator=(const Node &rhs)
{
    const SigningCast &rhs_cast = static_cast<const SigningCast &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool SigningCast::operator==(const SigningCast &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_signing() != rhs.get_signing()) {
        return false;
    }
    return true;
}

bool SigningCast::operator==(const Node &rhs) const
{
    const SigningCast &rhs_cast = static_cast<const SigningCast &>(rhs);
    return operator==(rhs_cast);
}

bool SigningCast::operator!=(const SigningCast &rhs) const { return !(operator==(rhs)); }

bool SigningCast::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool SigningCast::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool SigningCast::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_expr()) {
        if(get_expr() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "SigningCast::replace matches multiple times (Node::expr)";
            }
            set_expr(new_node);
            found = true;
        }
    }
    return found;
}

bool SigningCast::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

SigningCast::ListPtr SigningCast::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<SigningCast>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr SigningCast::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_expr()) {
        list->push_back(std::static_pointer_cast<Node>(get_expr()));
    }
    return list;
}

void SigningCast::clone_children(Node::Ptr new_node) const
{
    if(get_expr()) {
        cast_to<SigningCast>(new_node)->set_expr(get_expr()->clone());
    }
}

Node::Ptr SigningCast::alloc_same(void) const
{
    Ptr p(new SigningCast);
    return p;
}

std::ostream &operator<<(std::ostream &os, const SigningCast &p)
{
    os << "SigningCast: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "signing: " << p.get_signing();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SigningCast::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "SigningCast: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const SigningCast::SigningEnum p)
{
    switch(p) {
    case SigningCast::SigningEnum::SIGNED:
        os << "SIGNED";
        break;
    case SigningCast::SigningEnum::UNSIGNED:
        os << "UNSIGNED";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse