// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/declaration.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Declaration::Declaration(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Declaration);
    set_node_categories({NodeType::Node});
}

Declaration::Declaration(const DataType::Ptr type, const std::string &name,
                         const std::string &filename, uint32_t line)
    : Node(filename, line), m_type(type), m_name(name)
{
    set_node_type(NodeType::Declaration);
    set_node_categories({NodeType::Node});
}

Declaration &Declaration::operator=(const Declaration &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Declaration::operator=(const Node &rhs)
{
    const Declaration &rhs_cast = static_cast<const Declaration &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Declaration::operator==(const Declaration &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Declaration::operator==(const Node &rhs) const
{
    const Declaration &rhs_cast = static_cast<const Declaration &>(rhs);
    return operator==(rhs_cast);
}

bool Declaration::operator!=(const Declaration &rhs) const { return !(operator==(rhs)); }

bool Declaration::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Declaration::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Declaration::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Declaration::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool Declaration::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

Declaration::ListPtr Declaration::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Declaration>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Declaration::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void Declaration::clone_children(Node::Ptr new_node) const
{
    if(get_type()) {
        cast_to<Declaration>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr Declaration::alloc_same(void) const
{
    Ptr p(new Declaration);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Declaration &p)
{
    os << "Declaration: {";
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

std::ostream &operator<<(std::ostream &os, const Declaration::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Declaration: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse