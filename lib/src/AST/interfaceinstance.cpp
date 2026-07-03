// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/interfaceinstance.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

InterfaceInstance::InterfaceInstance(const std::string &filename, uint32_t line)
    : Instance(filename, line)
{
    set_node_type(NodeType::InterfaceInstance);
    set_node_categories({NodeType::Instance, NodeType::Node});
}

InterfaceInstance::InterfaceInstance(const Dimension::Ptr array,
                                     const ParamArg::ListPtr parameterlist,
                                     const PortArg::ListPtr portlist, const std::string &module,
                                     const std::string &name, const std::string &filename,
                                     uint32_t line)
    : Instance(array, parameterlist, portlist, module, name, filename, line)
{
    set_node_type(NodeType::InterfaceInstance);
    set_node_categories({NodeType::Instance, NodeType::Node});
}

InterfaceInstance &InterfaceInstance::operator=(const InterfaceInstance &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_module(rhs.get_module());
    set_name(rhs.get_name());
    return *this;
}

Node &InterfaceInstance::operator=(const Node &rhs)
{
    const InterfaceInstance &rhs_cast = static_cast<const InterfaceInstance &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool InterfaceInstance::operator==(const InterfaceInstance &rhs) const
{
    if(Node::operator==(rhs) == false) {
        return false;
    }
    if(get_module() != rhs.get_module()) {
        return false;
    }
    if(get_name() != rhs.get_name()) {
        return false;
    }
    return true;
}

bool InterfaceInstance::operator==(const Node &rhs) const
{
    const InterfaceInstance &rhs_cast = static_cast<const InterfaceInstance &>(rhs);
    return operator==(rhs_cast);
}

bool InterfaceInstance::operator!=(const InterfaceInstance &rhs) const
{
    return !(operator==(rhs));
}

bool InterfaceInstance::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool InterfaceInstance::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool InterfaceInstance::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_array()) {
        if(get_array() == node) {
            if(found) {
                LOG_WARNING
                    << *this << ", "
                    << "InterfaceInstance::replace matches multiple times (Dimension::array)";
            }
            set_array(cast_to<Dimension>(new_node));
            found = true;
        }
    }
    if(get_parameterlist()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_parameterlist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceInstance::replace matches multiple times "
                                       "(list(ParamArg)::parameterlist)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<ParamArg>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceInstance::replace "
                            << "of children list(ParamArg)::parameterlist";
            }
        }
        if(new_list->size() != 0) {
            set_parameterlist(new_list);
        } else {
            set_parameterlist(nullptr);
        }
    }
    if(get_portlist()) {
        PortArg::ListPtr new_list = std::make_shared<PortArg::List>();
        for(const PortArg::Ptr &lnode : *get_portlist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceInstance::replace matches multiple times "
                                       "(list(PortArg)::portlist)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<PortArg>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceInstance::replace "
                            << "of children list(PortArg)::portlist";
            }
        }
        if(new_list->size() != 0) {
            set_portlist(new_list);
        } else {
            set_portlist(nullptr);
        }
    }
    return found;
}

bool InterfaceInstance::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_parameterlist()) {
        ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
        for(const ParamArg::Ptr &lnode : *get_parameterlist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceInstance::replace matches multiple times "
                                       "(list(ParamArg)::parameterlist)";
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
                            << "found an empty node during InterfaceInstance::replace "
                            << "of children list(ParamArg)::parameterlist";
            }
        }
        if(new_list->size() != 0) {
            set_parameterlist(new_list);
        } else {
            set_parameterlist(nullptr);
        }
    }
    if(get_portlist()) {
        PortArg::ListPtr new_list = std::make_shared<PortArg::List>();
        for(const PortArg::Ptr &lnode : *get_portlist()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "InterfaceInstance::replace matches multiple times "
                                       "(list(PortArg)::portlist)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<PortArg>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during InterfaceInstance::replace "
                            << "of children list(PortArg)::portlist";
            }
        }
        if(new_list->size() != 0) {
            set_portlist(new_list);
        } else {
            set_portlist(nullptr);
        }
    }
    return found;
}

InterfaceInstance::ListPtr InterfaceInstance::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<InterfaceInstance>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr InterfaceInstance::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_array()) {
        list->push_back(std::static_pointer_cast<Node>(get_array()));
    }
    if(get_parameterlist()) {
        for(const ParamArg::Ptr &node : *get_parameterlist()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_portlist()) {
        for(const PortArg::Ptr &node : *get_portlist()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void InterfaceInstance::clone_children(Node::Ptr new_node) const
{
    if(get_array()) {
        cast_to<InterfaceInstance>(new_node)->set_array(cast_to<Dimension>(get_array()->clone()));
    }
    cast_to<InterfaceInstance>(new_node)->set_parameterlist(
        ParamArg::clone_list(get_parameterlist()));
    cast_to<InterfaceInstance>(new_node)->set_portlist(PortArg::clone_list(get_portlist()));
}

Node::Ptr InterfaceInstance::alloc_same(void) const
{
    Ptr p(new InterfaceInstance);
    return p;
}

std::ostream &operator<<(std::ostream &os, const InterfaceInstance &p)
{
    os << "InterfaceInstance: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "module: " << p.get_module() << ", ";

    os << "name: " << p.get_name();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const InterfaceInstance::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "InterfaceInstance: {nullptr}";
    }

    return os;
}

} // namespace AST
} // namespace Veriparse