// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/scopedref.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ScopedRef::ScopedRef(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::ScopedRef);
    set_node_categories({NodeType::Node});
}

ScopedRef::ScopedRef(const std::string &package, const std::string &name,
                     const std::string &filename, uint32_t line)
    : Node(filename, line), m_package(package), m_name(name)
{
    set_node_type(NodeType::ScopedRef);
    set_node_categories({NodeType::Node});
}

ScopedRef &ScopedRef::operator=(const ScopedRef &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_package(rhs.get_package());
    set_name(rhs.get_name());
    return *this;
}

Node &ScopedRef::operator=(const Node &rhs)
{
    const ScopedRef &rhs_cast = static_cast<const ScopedRef &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ScopedRef::operator==(const ScopedRef &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_package() != rhs.get_package()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool ScopedRef::operator==(const Node &rhs) const
{
    const ScopedRef &rhs_cast = static_cast<const ScopedRef &>(rhs);
    return operator==(rhs_cast);
}

bool ScopedRef::operator!=(const ScopedRef &rhs) const { return !(operator==(rhs)); }

bool ScopedRef::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ScopedRef::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ScopedRef::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool ScopedRef::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

ScopedRef::ListPtr ScopedRef::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ScopedRef>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ScopedRef::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void ScopedRef::clone_children(Node::Ptr new_node) const {}

Node::Ptr ScopedRef::alloc_same(void) const
{
    Ptr p(new ScopedRef);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ScopedRef &p)
{
    os << "ScopedRef: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "package: " << p.get_package() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ScopedRef::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ScopedRef: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse