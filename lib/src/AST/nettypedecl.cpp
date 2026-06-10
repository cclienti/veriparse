// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/nettypedecl.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

NetTypeDecl::NetTypeDecl(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::NetTypeDecl);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

NetTypeDecl::NetTypeDecl(const Identifier::Ptr resolver, const DataType::Ptr type,
                         const std::string &name, const std::string &filename, uint32_t line)
    : Declaration(type, name, filename, line), m_resolver(resolver)
{
    set_node_type(NodeType::NetTypeDecl);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

NetTypeDecl &NetTypeDecl::operator=(const NetTypeDecl &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &NetTypeDecl::operator=(const Node &rhs)
{
    const NetTypeDecl &rhs_cast = static_cast<const NetTypeDecl &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool NetTypeDecl::operator==(const NetTypeDecl &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool NetTypeDecl::operator==(const Node &rhs) const
{
    const NetTypeDecl &rhs_cast = static_cast<const NetTypeDecl &>(rhs);
    return operator==(rhs_cast);
}

bool NetTypeDecl::operator!=(const NetTypeDecl &rhs) const { return !(operator==(rhs)); }

bool NetTypeDecl::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool NetTypeDecl::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool NetTypeDecl::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_resolver()) {
        if(get_resolver() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "NetTypeDecl::replace matches multiple times (Identifier::resolver)";
            }
            set_resolver(cast_to<Identifier>(new_node));
            found = true;
        }
    }
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "NetTypeDecl::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool NetTypeDecl::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

NetTypeDecl::ListPtr NetTypeDecl::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<NetTypeDecl>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr NetTypeDecl::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_resolver()) {
        list->push_back(std::static_pointer_cast<Node>(get_resolver()));
    }
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void NetTypeDecl::clone_children(Node::Ptr new_node) const
{
    if(get_resolver()) {
        cast_to<NetTypeDecl>(new_node)->set_resolver(cast_to<Identifier>(get_resolver()->clone()));
    }
    if(get_type()) {
        cast_to<NetTypeDecl>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr NetTypeDecl::alloc_same(void) const
{
    Ptr p(new NetTypeDecl);
    return p;
}

std::ostream &operator<<(std::ostream &os, const NetTypeDecl &p)
{
    os << "NetTypeDecl: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const NetTypeDecl::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "NetTypeDecl: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse