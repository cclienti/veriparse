// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/hiername.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

HierName::HierName(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::HierName);
    set_node_categories({NodeType::Node});
}

HierName::HierName(const HierLabel::ListPtr labellist, const std::string &filename, uint32_t line)
    : Node(filename, line), m_labellist(labellist)
{
    set_node_type(NodeType::HierName);
    set_node_categories({NodeType::Node});
}

HierName &HierName::operator=(const HierName &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    return *this;
}

Node &HierName::operator=(const Node &rhs)
{
    const HierName &rhs_cast = static_cast<const HierName &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool HierName::operator==(const HierName &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    return true;
}

bool HierName::operator==(const Node &rhs) const
{
    const HierName &rhs_cast = static_cast<const HierName &>(rhs);
    return operator==(rhs_cast);
}

bool HierName::operator!=(const HierName &rhs) const { return !(operator==(rhs)); }

bool HierName::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool HierName::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool HierName::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_labellist()) {
        HierLabel::ListPtr new_list = std::make_shared<HierLabel::List>();
        for(const HierLabel::Ptr &lnode : *get_labellist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "HierName::replace matches multiple times "
                                       "(list(HierLabel)::labellist)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<HierLabel>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during HierName::replace "
                            << "of children list(HierLabel)::labellist";
            }
        }
        if(new_list->size() != 0) {
            set_labellist(new_list);
        } else {
            set_labellist(nullptr);
        }
    }
    return found;
}

bool HierName::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_labellist()) {
        HierLabel::ListPtr new_list = std::make_shared<HierLabel::List>();
        for(const HierLabel::Ptr &lnode : *get_labellist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "HierName::replace matches multiple times "
                                       "(list(HierLabel)::labellist)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<HierLabel>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during HierName::replace "
                            << "of children list(HierLabel)::labellist";
            }
        }
        if(new_list->size() != 0) {
            set_labellist(new_list);
        } else {
            set_labellist(nullptr);
        }
    }
    return found;
}

HierName::ListPtr HierName::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<HierName>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr HierName::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_labellist()) {
        for(const HierLabel::Ptr &node : *get_labellist()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void HierName::clone_children(Node::Ptr new_node) const
{
    cast_to<HierName>(new_node)->set_labellist(HierLabel::clone_list(get_labellist()));
}

Node::Ptr HierName::alloc_same(void) const
{
    Ptr p(new HierName);
    return p;
}

std::ostream &operator<<(std::ostream &os, const HierName &p)
{
    os << "HierName: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const HierName::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "HierName: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse