// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/call.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Call::Call(const std::string &filename, uint32_t line) : Identifier(filename, line)
{
    set_node_type(NodeType::Call);
    set_node_categories({NodeType::Identifier, NodeType::Node});
}

Call::Call(const Node::ListPtr args, const ScopeName::ListPtr scope, const HierName::Ptr hier,
           const std::string &name, const std::string &filename, uint32_t line)
    : Identifier(scope, hier, name, filename, line), m_args(args)
{
    set_node_type(NodeType::Call);
    set_node_categories({NodeType::Identifier, NodeType::Node});
}

Call &Call::operator=(const Call &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    return *this;
}

Node &Call::operator=(const Node &rhs)
{
    const Call &rhs_cast = static_cast<const Call &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Call::operator==(const Call &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool Call::operator==(const Node &rhs) const
{
    const Call &rhs_cast = static_cast<const Call &>(rhs);
    return operator==(rhs_cast);
}

bool Call::operator!=(const Call &rhs) const { return !(operator==(rhs)); }

bool Call::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Call::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Call::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_args()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_args()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Call::replace matches multiple times (list(Node)::args)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Call::replace "
                            << "of children list(Node)::args";
            }
        }
        if(new_list->size() != 0) {
            set_args(new_list);
        } else {
            set_args(nullptr);
        }
    }
    if(get_scope()) {
        ScopeName::ListPtr new_list = std::make_shared<ScopeName::List>();
        for(const ScopeName::Ptr &lnode : *get_scope()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Call::replace matches multiple times (list(ScopeName)::scope)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ScopeName>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Call::replace "
                            << "of children list(ScopeName)::scope";
            }
        }
        if(new_list->size() != 0) {
            set_scope(new_list);
        } else {
            set_scope(nullptr);
        }
    }
    if(get_hier()) {
        if(get_hier() == node) {
            if(found) {
                LOG_WARNING << *this << ", "
                            << "Call::replace matches multiple times (HierName::hier)";
            }
            set_hier(cast_to<HierName>(new_node));
            found = true;
        }
    }
    return found;
}

bool Call::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_args()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_args()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Call::replace matches multiple times (list(Node)::args)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(n);
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Call::replace "
                            << "of children list(Node)::args";
            }
        }
        if(new_list->size() != 0) {
            set_args(new_list);
        } else {
            set_args(nullptr);
        }
    }
    if(get_scope()) {
        ScopeName::ListPtr new_list = std::make_shared<ScopeName::List>();
        for(const ScopeName::Ptr &lnode : *get_scope()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Call::replace matches multiple times (list(ScopeName)::scope)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<ScopeName>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Call::replace "
                            << "of children list(ScopeName)::scope";
            }
        }
        if(new_list->size() != 0) {
            set_scope(new_list);
        } else {
            set_scope(nullptr);
        }
    }
    return found;
}

Call::ListPtr Call::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Call>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Call::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_args()) {
        for(const Node::Ptr &node : *get_args()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_scope()) {
        for(const ScopeName::Ptr &node : *get_scope()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_hier()) {
        list->push_back(std::static_pointer_cast<Node>(get_hier()));
    }
    return list;
}

void Call::clone_children(Node::Ptr new_node) const
{
    cast_to<Call>(new_node)->set_args(Node::clone_list(get_args()));
    cast_to<Call>(new_node)->set_scope(ScopeName::clone_list(get_scope()));
    if(get_hier()) {
        cast_to<Call>(new_node)->set_hier(cast_to<HierName>(get_hier()->clone()));
    }
}

Node::Ptr Call::alloc_same(void) const
{
    Ptr p(new Call);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Call &p)
{
    os << "Call: {";
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

std::ostream &operator<<(std::ostream &os, const Call::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Call: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse