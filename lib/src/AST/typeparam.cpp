// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/typeparam.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

TypeParam::TypeParam(const std::string &filename, uint32_t line) : Declaration(filename, line)
{
    set_node_type(NodeType::TypeParam);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

TypeParam::TypeParam(const DataType::Ptr type, const bool &is_local, const std::string &name,
                     const std::string &filename, uint32_t line)
    : Declaration(type, name, filename, line), m_is_local(is_local)
{
    set_node_type(NodeType::TypeParam);
    set_node_categories({NodeType::Declaration, NodeType::Node});
}

TypeParam &TypeParam::operator=(const TypeParam &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_local(rhs.get_is_local());
    set_name(rhs.get_name());
    return *this;
}

Node &TypeParam::operator=(const Node &rhs)
{
    const TypeParam &rhs_cast = static_cast<const TypeParam &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool TypeParam::operator==(const TypeParam &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_local() != rhs.get_is_local()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool TypeParam::operator==(const Node &rhs) const
{
    const TypeParam &rhs_cast = static_cast<const TypeParam &>(rhs);
    return operator==(rhs_cast);
}

bool TypeParam::operator!=(const TypeParam &rhs) const { return !(operator==(rhs)); }

bool TypeParam::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool TypeParam::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool TypeParam::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "TypeParam::replace matches multiple times (DataType::type)";
            }
            set_type(cast_to<DataType>(new_node));
            found = true;
        }
    }
    return found;
}

bool TypeParam::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

TypeParam::ListPtr TypeParam::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<TypeParam>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr TypeParam::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    return list;
}

void TypeParam::clone_children(Node::Ptr new_node) const
{
    if(get_type()) {
        cast_to<TypeParam>(new_node)->set_type(cast_to<DataType>(get_type()->clone()));
    }
}

Node::Ptr TypeParam::alloc_same(void) const
{
    Ptr p(new TypeParam);
    return p;
}

std::ostream &operator<<(std::ostream &os, const TypeParam &p)
{
    os << "TypeParam: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_local: " << p.get_is_local() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const TypeParam::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "TypeParam: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse