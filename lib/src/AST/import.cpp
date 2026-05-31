// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/import.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Import::Import(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Import);
    set_node_categories({NodeType::Node});
}

Import::Import(const std::string &package, const std::string &symbol, const std::string &filename,
               uint32_t line)
    : Node(filename, line), m_package(package), m_symbol(symbol)
{
    set_node_type(NodeType::Import);
    set_node_categories({NodeType::Node});
}

Import &Import::operator=(const Import &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_package(rhs.get_package());
    set_symbol(rhs.get_symbol());
    return *this;
}

Node &Import::operator=(const Node &rhs)
{
    const Import &rhs_cast = static_cast<const Import &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Import::operator==(const Import &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_package() != rhs.get_package()) {
        return false;
    }
    if(get_symbol() != rhs.get_symbol()) {
        return false;
    }
    return true;
}

bool Import::operator==(const Node &rhs) const
{
    const Import &rhs_cast = static_cast<const Import &>(rhs);
    return operator==(rhs_cast);
}

bool Import::operator!=(const Import &rhs) const { return !(operator==(rhs)); }

bool Import::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Import::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Import::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool Import::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Import::ListPtr Import::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Import>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Import::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void Import::clone_children(Node::Ptr new_node) const {}

Node::Ptr Import::alloc_same(void) const
{
    Ptr p(new Import);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Import &p)
{
    os << "Import: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "package: " << p.get_package() << ", ";

    os << "symbol: " << p.get_symbol();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Import::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Import: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse