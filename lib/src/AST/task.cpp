// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/task.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Task::Task(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Task);
    set_node_categories({NodeType::Node});
}

Task::Task(const Arg::ListPtr args, const Node::ListPtr statements, const std::string &name,
           const LifetimeEnum &lifetime, const std::string &filename, uint32_t line)
    : Node(filename, line), m_args(args), m_statements(statements), m_name(name),
      m_lifetime(lifetime)
{
    set_node_type(NodeType::Task);
    set_node_categories({NodeType::Node});
}

Task &Task::operator=(const Task &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_lifetime(rhs.get_lifetime());
    return *this;
}

Node &Task::operator=(const Node &rhs)
{
    const Task &rhs_cast = static_cast<const Task &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Task::operator==(const Task &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    if(get_lifetime() != rhs.get_lifetime()) {
        return false;
    }
    return true;
}

bool Task::operator==(const Node &rhs) const
{
    const Task &rhs_cast = static_cast<const Task &>(rhs);
    return operator==(rhs_cast);
}

bool Task::operator!=(const Task &rhs) const { return !(operator==(rhs)); }

bool Task::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Task::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Task::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_args()) {
        Arg::ListPtr new_list = std::make_shared<Arg::List>();
        for(const Arg::Ptr &lnode : *get_args()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Task::replace matches multiple times (list(Arg)::args)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Arg>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Task::replace "
                            << "of children list(Arg)::args";
            }
        }
        if(new_list->size() != 0) {
            set_args(new_list);
        } else {
            set_args(nullptr);
        }
    }
    if(get_statements()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_statements()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Task::replace matches multiple times (list(Node)::statements)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Task::replace "
                            << "of children list(Node)::statements";
            }
        }
        if(new_list->size() != 0) {
            set_statements(new_list);
        } else {
            set_statements(nullptr);
        }
    }
    return found;
}

bool Task::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_args()) {
        Arg::ListPtr new_list = std::make_shared<Arg::List>();
        for(const Arg::Ptr &lnode : *get_args()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Task::replace matches multiple times (list(Arg)::args)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Arg>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Task::replace "
                            << "of children list(Arg)::args";
            }
        }
        if(new_list->size() != 0) {
            set_args(new_list);
        } else {
            set_args(nullptr);
        }
    }
    if(get_statements()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_statements()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Task::replace matches multiple times (list(Node)::statements)";
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
                            << "found an empty node during Task::replace "
                            << "of children list(Node)::statements";
            }
        }
        if(new_list->size() != 0) {
            set_statements(new_list);
        } else {
            set_statements(nullptr);
        }
    }
    return found;
}

Task::ListPtr Task::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Task>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Task::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_args()) {
        for(const Arg::Ptr &node : *get_args()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_statements()) {
        for(const Node::Ptr &node : *get_statements()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void Task::clone_children(Node::Ptr new_node) const
{
    cast_to<Task>(new_node)->set_args(Arg::clone_list(get_args()));
    cast_to<Task>(new_node)->set_statements(Node::clone_list(get_statements()));
}

Node::Ptr Task::alloc_same(void) const
{
    Ptr p(new Task);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Task &p)
{
    os << "Task: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "lifetime: " << p.get_lifetime();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Task::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Task: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Task::LifetimeEnum p)
{
    switch(p) {
    case Task::LifetimeEnum::NONE:
        os << "NONE";
        break;
    case Task::LifetimeEnum::AUTOMATIC:
        os << "AUTOMATIC";
        break;
    case Task::LifetimeEnum::STATIC:
        os << "STATIC";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse