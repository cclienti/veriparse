// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/chargestrength.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ChargeStrength::ChargeStrength(const std::string &filename, uint32_t line)
    : Strength(filename, line)
{
    set_node_type(NodeType::ChargeStrength);
    set_node_categories({NodeType::Strength, NodeType::Node});
}

ChargeStrength::ChargeStrength(const ChargeEnum &charge, const std::string &filename, uint32_t line)
    : Strength(filename, line), m_charge(charge)
{
    set_node_type(NodeType::ChargeStrength);
    set_node_categories({NodeType::Strength, NodeType::Node});
}

ChargeStrength &ChargeStrength::operator=(const ChargeStrength &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_charge(rhs.get_charge());
    return *this;
}

Node &ChargeStrength::operator=(const Node &rhs)
{
    const ChargeStrength &rhs_cast = static_cast<const ChargeStrength &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ChargeStrength::operator==(const ChargeStrength &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_charge() != rhs.get_charge()) {
        return false;
    }
    return true;
}

bool ChargeStrength::operator==(const Node &rhs) const
{
    const ChargeStrength &rhs_cast = static_cast<const ChargeStrength &>(rhs);
    return operator==(rhs_cast);
}

bool ChargeStrength::operator!=(const ChargeStrength &rhs) const { return !(operator==(rhs)); }

bool ChargeStrength::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ChargeStrength::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ChargeStrength::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    return found;
}

bool ChargeStrength::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

ChargeStrength::ListPtr ChargeStrength::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ChargeStrength>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ChargeStrength::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    return list;
}

void ChargeStrength::clone_children(Node::Ptr new_node) const {}

Node::Ptr ChargeStrength::alloc_same(void) const
{
    Ptr p(new ChargeStrength);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ChargeStrength &p)
{
    os << "ChargeStrength: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "charge: " << p.get_charge();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ChargeStrength::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ChargeStrength: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const ChargeStrength::ChargeEnum p)
{
    switch(p) {
    case ChargeStrength::ChargeEnum::SMALL:
        os << "SMALL";
        break;
    case ChargeStrength::ChargeEnum::MEDIUM:
        os << "MEDIUM";
        break;
    case ChargeStrength::ChargeEnum::LARGE:
        os << "LARGE";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse