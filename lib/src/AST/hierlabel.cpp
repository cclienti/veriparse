// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/hierlabel.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

HierLabel::HierLabel(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::HierLabel);
    set_node_categories({NodeType::Node});
}

HierLabel::HierLabel(const Node::Ptr loop, const std::string &name, const std::string &filename,
                     uint32_t line)
    : Node(filename, line), m_loop(loop), m_name(name)
{
    set_node_type(NodeType::HierLabel);
    set_node_categories({NodeType::Node});
}

HierLabel &HierLabel::operator=(const HierLabel &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &HierLabel::operator=(const Node &rhs)
{
    const HierLabel &rhs_cast = static_cast<const HierLabel &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool HierLabel::operator==(const HierLabel &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool HierLabel::operator==(const Node &rhs) const
{
    const HierLabel &rhs_cast = static_cast<const HierLabel &>(rhs);
    return operator==(rhs_cast);
}

bool HierLabel::operator!=(const HierLabel &rhs) const { return !(operator==(rhs)); }

bool HierLabel::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool HierLabel::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool HierLabel::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_loop()) {
        if(get_loop() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "HierLabel::replace matches multiple times (Node::loop)";
            }
            set_loop(new_node);
            found = true;
        }
    }
    return found;
}

bool HierLabel::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    return found;
}

HierLabel::ListPtr HierLabel::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<HierLabel>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr HierLabel::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_loop()) {
        list->push_back(std::static_pointer_cast<Node>(get_loop()));
    }
    return list;
}

void HierLabel::clone_children(Node::Ptr new_node) const
{
    if(get_loop()) {
        cast_to<HierLabel>(new_node)->set_loop(get_loop()->clone());
    }
}

Node::Ptr HierLabel::alloc_same(void) const
{
    Ptr p(new HierLabel);
    return p;
}

std::ostream &operator<<(std::ostream &os, const HierLabel &p)
{
    os << "HierLabel: {";
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

std::ostream &operator<<(std::ostream &os, const HierLabel::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "HierLabel: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse