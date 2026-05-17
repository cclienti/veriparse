// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/enumdef.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

EnumDef::EnumDef(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::EnumDef);
    set_node_categories({NodeType::Node});
}

EnumDef::EnumDef(const Width::ListPtr widths, const EnumItem::ListPtr items,
                 const Base_typeEnum &base_type, const bool &sign, const std::string &filename,
                 uint32_t line)
    : Node(filename, line), m_widths(widths), m_items(items), m_base_type(base_type), m_sign(sign)
{
    set_node_type(NodeType::EnumDef);
    set_node_categories({NodeType::Node});
}

EnumDef &EnumDef::operator=(const EnumDef &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_base_type(rhs.get_base_type());
    set_sign(rhs.get_sign());
    return *this;
}

Node &EnumDef::operator=(const Node &rhs)
{
    const EnumDef &rhs_cast = static_cast<const EnumDef &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool EnumDef::operator==(const EnumDef &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_base_type() != rhs.get_base_type()) {
        return false;
    }
    if(get_sign() != rhs.get_sign()) {
        return false;
    }
    return true;
}

bool EnumDef::operator==(const Node &rhs) const
{
    const EnumDef &rhs_cast = static_cast<const EnumDef &>(rhs);
    return operator==(rhs_cast);
}

bool EnumDef::operator!=(const EnumDef &rhs) const { return !(operator==(rhs)); }

bool EnumDef::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool EnumDef::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool EnumDef::replace(Node::Ptr node, Node::Ptr new_node)
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
                        LOG_WARNING
                            << *this << ", "
                            << "EnumDef::replace matches multiple times (list(Width)::widths)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Width>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumDef::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
        }
    }
    if(get_items()) {
        EnumItem::ListPtr new_list = std::make_shared<EnumItem::List>();
        for(const EnumItem::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "EnumDef::replace matches multiple times (list(EnumItem)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<EnumItem>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumDef::replace "
                            << "of children list(EnumItem)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
        }
    }
    return found;
}

bool EnumDef::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                        LOG_WARNING
                            << *this << ", "
                            << "EnumDef::replace matches multiple times (list(Width)::widths)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes)
                            new_list->push_back(cast_to<Width>(n));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumDef::replace "
                            << "of children list(Width)::widths";
            }
        }
        if(new_list->size() != 0) {
            set_widths(new_list);
        } else {
            set_widths(nullptr);
        }
    }
    if(get_items()) {
        EnumItem::ListPtr new_list = std::make_shared<EnumItem::List>();
        for(const EnumItem::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "EnumDef::replace matches multiple times (list(EnumItem)::items)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes)
                            new_list->push_back(cast_to<EnumItem>(n));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during EnumDef::replace "
                            << "of children list(EnumItem)::items";
            }
        }
        if(new_list->size() != 0) {
            set_items(new_list);
        } else {
            set_items(nullptr);
        }
    }
    return found;
}

EnumDef::ListPtr EnumDef::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<EnumDef>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr EnumDef::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_widths()) {
        for(const Width::Ptr &node : *get_widths()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_items()) {
        for(const EnumItem::Ptr &node : *get_items()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void EnumDef::clone_children(Node::Ptr new_node) const
{
    cast_to<EnumDef>(new_node)->set_widths(Width::clone_list(get_widths()));
    cast_to<EnumDef>(new_node)->set_items(EnumItem::clone_list(get_items()));
}

Node::Ptr EnumDef::alloc_same(void) const
{
    Ptr p(new EnumDef);
    return p;
}

std::ostream &operator<<(std::ostream &os, const EnumDef &p)
{
    os << "EnumDef: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty())
        os << ", ";

    os << "base_type: " << p.get_base_type() << ", ";

    os << "sign: " << p.get_sign();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const EnumDef::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "EnumDef: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const EnumDef::Base_typeEnum p)
{
    switch(p) {
    case EnumDef::Base_typeEnum::LOGIC:
        os << "LOGIC";
        break;
    case EnumDef::Base_typeEnum::BIT:
        os << "BIT";
        break;
    case EnumDef::Base_typeEnum::INT:
        os << "INT";
        break;
    case EnumDef::Base_typeEnum::NONE:
        os << "NONE";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse