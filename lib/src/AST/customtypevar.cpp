// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/customtypevar.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

CustomTypeVar::CustomTypeVar(const std::string &filename, uint32_t line) : Variable(filename, line)
{
    set_node_type(NodeType::CustomTypeVar);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

CustomTypeVar::CustomTypeVar(const Node::Ptr type, const Width::ListPtr widths,
                             const Length::ListPtr lengths, const Rvalue::Ptr right,
                             const std::string &name, const std::string &filename, uint32_t line)
    : Variable(lengths, right, name, filename, line), m_type(type), m_widths(widths)
{
    set_node_type(NodeType::CustomTypeVar);
    set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
}

CustomTypeVar &CustomTypeVar::operator=(const CustomTypeVar &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &CustomTypeVar::operator=(const Node &rhs)
{
    const CustomTypeVar &rhs_cast = static_cast<const CustomTypeVar &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool CustomTypeVar::operator==(const CustomTypeVar &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool CustomTypeVar::operator==(const Node &rhs) const
{
    const CustomTypeVar &rhs_cast = static_cast<const CustomTypeVar &>(rhs);
    return operator==(rhs_cast);
}

bool CustomTypeVar::operator!=(const CustomTypeVar &rhs) const { return !(operator==(rhs)); }

bool CustomTypeVar::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool CustomTypeVar::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool CustomTypeVar::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_type()) {
        if(get_type() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "CustomTypeVar::replace matches multiple times (Node::type)";
            }
            set_type(new_node);
            found = true;
        }
    }
    if(get_widths()) {
        Width::ListPtr new_list = std::make_shared<Width::List>();
        for(const Width::Ptr &lnode : *get_widths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "CustomTypeVar::replace matches multiple times "
                                       "(list(Width)::widths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Width>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during CustomTypeVar::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
        }
    }
    if(get_lengths()) {
        Length::ListPtr new_list = std::make_shared<Length::List>();
        for(const Length::Ptr &lnode : *get_lengths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "CustomTypeVar::replace matches multiple times "
                                       "(list(Length)::lengths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Length>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during CustomTypeVar::replace "
                            << "of children list(Length)::lengths";
            }
        }
        if(new_list->size() != 0) {
            set_lengths(new_list);
        } else {
            set_lengths(nullptr);
        }
    }
    if(get_right()) {
        if(get_right() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "CustomTypeVar::replace matches multiple times (Rvalue::right)";
            }
            set_right(cast_to<Rvalue>(new_node));
            found = true;
        }
    }
    return found;
}

bool CustomTypeVar::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_widths()) {
        Width::ListPtr new_list = std::make_shared<Width::List>();
        for(const Width::Ptr &lnode : *get_widths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "CustomTypeVar::replace matches multiple times "
                                       "(list(Width)::widths)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Width>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during CustomTypeVar::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
        }
    }
    if(get_lengths()) {
        Length::ListPtr new_list = std::make_shared<Length::List>();
        for(const Length::Ptr &lnode : *get_lengths()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "CustomTypeVar::replace matches multiple times "
                                       "(list(Length)::lengths)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Length>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during CustomTypeVar::replace "
                            << "of children list(Length)::lengths";
            }
        }
        if(new_list->size() != 0) {
            set_lengths(new_list);
        } else {
            set_lengths(nullptr);
        }
    }
    return found;
}

CustomTypeVar::ListPtr CustomTypeVar::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<CustomTypeVar>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr CustomTypeVar::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_type()) {
        list->push_back(std::static_pointer_cast<Node>(get_type()));
    }
    if(get_widths()) {
        for(const Width::Ptr &node : *get_widths()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_lengths()) {
        for(const Length::Ptr &node : *get_lengths()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_right()) {
        list->push_back(std::static_pointer_cast<Node>(get_right()));
    }
    return list;
}

void CustomTypeVar::clone_children(Node::Ptr new_node) const
{
    if(get_type()) {
        cast_to<CustomTypeVar>(new_node)->set_type(get_type()->clone());
    }
    cast_to<CustomTypeVar>(new_node)->set_widths(Width::clone_list(get_widths()));
    cast_to<CustomTypeVar>(new_node)->set_lengths(Length::clone_list(get_lengths()));
    if(get_right()) {
        cast_to<CustomTypeVar>(new_node)->set_right(cast_to<Rvalue>(get_right()->clone()));
    }
}

Node::Ptr CustomTypeVar::alloc_same(void) const
{
    Ptr p(new CustomTypeVar);
    return p;
}

std::ostream &operator<<(std::ostream &os, const CustomTypeVar &p)
{
    os << "CustomTypeVar: {";
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

std::ostream &operator<<(std::ostream &os, const CustomTypeVar::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "CustomTypeVar: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse