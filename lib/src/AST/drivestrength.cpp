// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/drivestrength.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

DriveStrength::DriveStrength(const std::string &filename, uint32_t line) : Strength(filename, line)
{
    set_node_type(NodeType::DriveStrength);
    set_node_categories({NodeType::Strength, NodeType::Node});
}

DriveStrength::DriveStrength(const S0Enum &s0, const S1Enum &s1, const std::string &filename,
                             uint32_t line)
    : Strength(filename, line), m_s0(s0), m_s1(s1)
{
    set_node_type(NodeType::DriveStrength);
    set_node_categories({NodeType::Strength, NodeType::Node});
}

DriveStrength &DriveStrength::operator=(const DriveStrength &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_s0(rhs.get_s0());
    set_s1(rhs.get_s1());
    return *this;
}

Node &DriveStrength::operator=(const Node &rhs)
{
    const DriveStrength &rhs_cast = static_cast<const DriveStrength &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool DriveStrength::operator==(const DriveStrength &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_s0() != rhs.get_s0()) {
        return false;
    }
    if(get_s1() != rhs.get_s1()) {
        return false;
    }
    return true;
}

bool DriveStrength::operator==(const Node &rhs) const
{
    const DriveStrength &rhs_cast = static_cast<const DriveStrength &>(rhs);
    return operator==(rhs_cast);
}

bool DriveStrength::operator!=(const DriveStrength &rhs) const { return !(operator==(rhs)); }

bool DriveStrength::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool DriveStrength::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool DriveStrength::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool DriveStrength::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

DriveStrength::ListPtr DriveStrength::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<DriveStrength>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr DriveStrength::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void DriveStrength::clone_children(Node::Ptr new_node) const {}

Node::Ptr DriveStrength::alloc_same(void) const
{
    Ptr p(new DriveStrength);
    return p;
}

std::ostream &operator<<(std::ostream &os, const DriveStrength &p)
{
    os << "DriveStrength: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "s0: " << p.get_s0() << ", ";

    os << "s1: " << p.get_s1();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const DriveStrength::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "DriveStrength: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const DriveStrength::S0Enum p)
{
    switch(p) {
    case DriveStrength::S0Enum::SUPPLY0:
        os << "SUPPLY0";
        break;
    case DriveStrength::S0Enum::STRONG0:
        os << "STRONG0";
        break;
    case DriveStrength::S0Enum::PULL0:
        os << "PULL0";
        break;
    case DriveStrength::S0Enum::WEAK0:
        os << "WEAK0";
        break;
    case DriveStrength::S0Enum::HIGHZ0:
        os << "HIGHZ0";
        break;
    default:
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const DriveStrength::S1Enum p)
{
    switch(p) {
    case DriveStrength::S1Enum::SUPPLY1:
        os << "SUPPLY1";
        break;
    case DriveStrength::S1Enum::STRONG1:
        os << "STRONG1";
        break;
    case DriveStrength::S1Enum::PULL1:
        os << "PULL1";
        break;
    case DriveStrength::S1Enum::WEAK1:
        os << "WEAK1";
        break;
    case DriveStrength::S1Enum::HIGHZ1:
        os << "HIGHZ1";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse