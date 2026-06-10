// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/scopename.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

ScopeName::ScopeName(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::ScopeName);
    set_node_categories({NodeType::Node});
}

ScopeName::ScopeName(const ParamArg::ListPtr params, const std::string &name,
                     const std::string &filename, uint32_t line)
    : Node(filename, line), m_params(params), m_name(name)
{
    set_node_type(NodeType::ScopeName);
    set_node_categories({NodeType::Node});
}

ScopeName &ScopeName::operator=(const ScopeName &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &ScopeName::operator=(const Node &rhs)
{
    const ScopeName &rhs_cast = static_cast<const ScopeName &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool ScopeName::operator==(const ScopeName &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool ScopeName::operator==(const Node &rhs) const
{
    const ScopeName &rhs_cast = static_cast<const ScopeName &>(rhs);
    return operator==(rhs_cast);
}

bool ScopeName::operator!=(const ScopeName &rhs) const { return !(operator==(rhs)); }

bool ScopeName::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool ScopeName::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool ScopeName::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_params()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "ScopeName::replace matches multiple times (list(ParamArg)::params)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ParamArg>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during ScopeName::replace "
                            << "of children list(ParamArg)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
        }
    }
    return found;
}

bool ScopeName::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_params()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "ScopeName::replace matches multiple times (list(ParamArg)::params)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<ParamArg>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during ScopeName::replace "
                            << "of children list(ParamArg)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
        }
    }
    return found;
}

ScopeName::ListPtr ScopeName::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<ScopeName>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr ScopeName::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_params()) {
        for(const ParamArg::Ptr &node : *get_params()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void ScopeName::clone_children(Node::Ptr new_node) const
{
    cast_to<ScopeName>(new_node)->set_params(ParamArg::clone_list(get_params()));
}

Node::Ptr ScopeName::alloc_same(void) const
{
    Ptr p(new ScopeName);
    return p;
}

std::ostream &operator<<(std::ostream &os, const ScopeName &p)
{
    os << "ScopeName: {";
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

std::ostream &operator<<(std::ostream &os, const ScopeName::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "ScopeName: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse