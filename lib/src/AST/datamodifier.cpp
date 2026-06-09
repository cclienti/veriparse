// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/datamodifier.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

DataModifier::DataModifier(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::DataModifier);
    set_node_categories({NodeType::Node});
}

DataModifier::DataModifier(const Node::Ptr datatype, const bool &is_var, const bool &is_const,
                           const LifetimeEnum &lifetime, const std::string &filename, uint32_t line)
    : Node(filename, line), m_datatype(datatype), m_is_var(is_var), m_is_const(is_const),
      m_lifetime(lifetime)
{
    set_node_type(NodeType::DataModifier);
    set_node_categories({NodeType::Node});
}

DataModifier &DataModifier::operator=(const DataModifier &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_is_var(rhs.get_is_var());
    set_is_const(rhs.get_is_const());
    set_lifetime(rhs.get_lifetime());
    return *this;
}

Node &DataModifier::operator=(const Node &rhs)
{
    const DataModifier &rhs_cast = static_cast<const DataModifier &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool DataModifier::operator==(const DataModifier &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_is_var() != rhs.get_is_var()) {
        return false;
    }
    if(get_is_const() != rhs.get_is_const()) {
        return false;
    }
    if(get_lifetime() != rhs.get_lifetime()) {
        return false;
    }
    return true;
}

bool DataModifier::operator==(const Node &rhs) const
{
    const DataModifier &rhs_cast = static_cast<const DataModifier &>(rhs);
    return operator==(rhs_cast);
}

bool DataModifier::operator!=(const DataModifier &rhs) const { return !(operator==(rhs)); }

bool DataModifier::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool DataModifier::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool DataModifier::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_datatype()) {
        if(get_datatype() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "DataModifier::replace matches multiple times (Node::datatype)";
            }
            set_datatype(new_node);
            found = true;
        }
    }
    return found;
}

bool DataModifier::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

DataModifier::ListPtr DataModifier::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<DataModifier>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr DataModifier::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_datatype()) {
        list->push_back(std::static_pointer_cast<Node>(get_datatype()));
    }
    return list;
}

void DataModifier::clone_children(Node::Ptr new_node) const
{
    if(get_datatype()) {
        cast_to<DataModifier>(new_node)->set_datatype(get_datatype()->clone());
    }
}

Node::Ptr DataModifier::alloc_same(void) const
{
    Ptr p(new DataModifier);
    return p;
}

std::ostream &operator<<(std::ostream &os, const DataModifier &p)
{
    os << "DataModifier: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "is_var: " << p.get_is_var() << ", ";

    os << "is_const: " << p.get_is_const() << ", ";

    os << "lifetime: " << p.get_lifetime();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const DataModifier::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "DataModifier: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const DataModifier::LifetimeEnum p)
{
    switch(p) {
    case DataModifier::LifetimeEnum::NONE:
        os << "NONE";
        break;
    case DataModifier::LifetimeEnum::AUTOMATIC:
        os << "AUTOMATIC";
        break;
    case DataModifier::LifetimeEnum::STATIC:
        os << "STATIC";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse