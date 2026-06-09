// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/customtype.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

CustomType::CustomType(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::CustomType);
    set_node_categories({NodeType::Node});
}

CustomType::CustomType(const std::string &name, const std::string &package,
                       const std::string &filename, uint32_t line)
    : Node(filename, line), m_name(name), m_package(package)
{
    set_node_type(NodeType::CustomType);
    set_node_categories({NodeType::Node});
}

CustomType &CustomType::operator=(const CustomType &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_package(rhs.get_package());
    return *this;
}

Node &CustomType::operator=(const Node &rhs)
{
    const CustomType &rhs_cast = static_cast<const CustomType &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool CustomType::operator==(const CustomType &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    if(get_package() != rhs.get_package()) {
        return false;
    }
    return true;
}

bool CustomType::operator==(const Node &rhs) const
{
    const CustomType &rhs_cast = static_cast<const CustomType &>(rhs);
    return operator==(rhs_cast);
}

bool CustomType::operator!=(const CustomType &rhs) const { return !(operator==(rhs)); }

bool CustomType::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool CustomType::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool CustomType::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool CustomType::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

CustomType::ListPtr CustomType::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<CustomType>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr CustomType::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void CustomType::clone_children(Node::Ptr new_node) const {}

Node::Ptr CustomType::alloc_same(void) const
{
    Ptr p(new CustomType);
    return p;
}

std::ostream &operator<<(std::ostream &os, const CustomType &p)
{
    os << "CustomType: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "package: " << p.get_package();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const CustomType::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "CustomType: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse