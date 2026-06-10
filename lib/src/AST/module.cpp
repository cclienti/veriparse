// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/module.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Module::Module(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Module);
    set_node_categories({NodeType::Node});
}

Module::Module(const Declaration::ListPtr params, const Port::ListPtr ports,
               const Node::ListPtr items, const std::string &name, const LifetimeEnum &lifetime,
               const Default_nettypeEnum &default_nettype, const std::string &filename,
               uint32_t line)
    : Node(filename, line), m_params(params), m_ports(ports), m_items(items), m_name(name),
      m_lifetime(lifetime), m_default_nettype(default_nettype)
{
    set_node_type(NodeType::Module);
    set_node_categories({NodeType::Node});
}

Module &Module::operator=(const Module &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_lifetime(rhs.get_lifetime());
    set_default_nettype(rhs.get_default_nettype());
    return *this;
}

Node &Module::operator=(const Node &rhs)
{
    const Module &rhs_cast = static_cast<const Module &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Module::operator==(const Module &rhs) const
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
    if(get_default_nettype() != rhs.get_default_nettype()) {
        return false;
    }
    return true;
}

bool Module::operator==(const Node &rhs) const
{
    const Module &rhs_cast = static_cast<const Module &>(rhs);
    return operator==(rhs_cast);
}

bool Module::operator!=(const Module &rhs) const { return !(operator==(rhs)); }

bool Module::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Module::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Module::replace(Node::Ptr node, Node::Ptr new_node)
{
    bool found = false;
    if(get_params()) {
        Declaration::ListPtr new_list = std::make_shared<Declaration::List>();
        for(const Declaration::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Module::replace matches multiple times (list(Declaration)::params)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Declaration>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Module::replace "
                            << "of children list(Declaration)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
        }
    }
    if(get_ports()) {
        Port::ListPtr new_list = std::make_shared<Port::List>();
        for(const Port::Ptr &lnode : *get_ports()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Module::replace matches multiple times (list(Port)::ports)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Port>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Module::replace "
                            << "of children list(Port)::ports";
            }
        }
        if(new_list->size() != 0) {
            set_ports(new_list);
        } else {
            set_ports(nullptr);
        }
    }
    if(get_items()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Module::replace matches multiple times (list(Node)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Module::replace "
                            << "of children list(Node)::items";
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

bool Module::replace(Node::Ptr node, Node::ListPtr new_nodes)
{
    bool found = false;
    if(get_params()) {
        Declaration::ListPtr new_list = std::make_shared<Declaration::List>();
        for(const Declaration::Ptr &lnode : *get_params()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING
                            << *this << ", "
                            << "Module::replace matches multiple times (list(Declaration)::params)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Declaration>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Module::replace "
                            << "of children list(Declaration)::params";
            }
        }
        if(new_list->size() != 0) {
            set_params(new_list);
        } else {
            set_params(nullptr);
        }
    }
    if(get_ports()) {
        Port::ListPtr new_list = std::make_shared<Port::List>();
        for(const Port::Ptr &lnode : *get_ports()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Module::replace matches multiple times (list(Port)::ports)";
                    }
                    if(new_nodes) {
                        for(const Node::Ptr &n : *new_nodes) {
                            new_list->push_back(cast_to<Port>(n));
                        }
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Module::replace "
                            << "of children list(Port)::ports";
            }
        }
        if(new_list->size() != 0) {
            set_ports(new_list);
        } else {
            set_ports(nullptr);
        }
    }
    if(get_items()) {
        Node::ListPtr new_list = std::make_shared<Node::List>();
        for(const Node::Ptr &lnode : *get_items()) {
            if(lnode) {
                if(lnode != node) {
                    new_list->push_back(lnode);
                } else {
                    if(found) {
                        LOG_WARNING << *this << ", "
                                    << "Module::replace matches multiple times (list(Node)::items)";
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
                            << "found an empty node during Module::replace "
                            << "of children list(Node)::items";
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

Module::ListPtr Module::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Module>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Module::get_children(void) const
{
    Node::ListPtr list = std::make_shared<Node::List>();
    if(get_params()) {
        for(const Declaration::Ptr &node : *get_params()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_ports()) {
        for(const Port::Ptr &node : *get_ports()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    if(get_items()) {
        for(const Node::Ptr &node : *get_items()) {
            if(node) {
                list->push_back(std::static_pointer_cast<Node>(node));
            }
        }
    }
    return list;
}

void Module::clone_children(Node::Ptr new_node) const
{
    cast_to<Module>(new_node)->set_params(Declaration::clone_list(get_params()));
    cast_to<Module>(new_node)->set_ports(Port::clone_list(get_ports()));
    cast_to<Module>(new_node)->set_items(Node::clone_list(get_items()));
}

Node::Ptr Module::alloc_same(void) const
{
    Ptr p(new Module);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Module &p)
{
    os << "Module: {";
    if(!p.get_filename().empty()) {
        os << "filename: " << p.get_filename() << ", "
           << "line: " << p.get_line();
    }

    if(!p.get_filename().empty()) {
        os << ", ";
    }

    os << "name: " << p.get_name() << ", ";

    os << "lifetime: " << p.get_lifetime() << ", ";

    os << "default_nettype: " << p.get_default_nettype();
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Module::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Module: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Module::LifetimeEnum p)
{
    switch(p) {
    case Module::LifetimeEnum::NONE:
        os << "NONE";
        break;
    case Module::LifetimeEnum::AUTOMATIC:
        os << "AUTOMATIC";
        break;
    case Module::LifetimeEnum::STATIC:
        os << "STATIC";
        break;
    default:
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Module::Default_nettypeEnum p)
{
    switch(p) {
    case Module::Default_nettypeEnum::WIRE:
        os << "WIRE";
        break;
    case Module::Default_nettypeEnum::TRI:
        os << "TRI";
        break;
    case Module::Default_nettypeEnum::TRI0:
        os << "TRI0";
        break;
    case Module::Default_nettypeEnum::TRI1:
        os << "TRI1";
        break;
    case Module::Default_nettypeEnum::TRIAND:
        os << "TRIAND";
        break;
    case Module::Default_nettypeEnum::TRIOR:
        os << "TRIOR";
        break;
    case Module::Default_nettypeEnum::TRIREG:
        os << "TRIREG";
        break;
    case Module::Default_nettypeEnum::WAND:
        os << "WAND";
        break;
    case Module::Default_nettypeEnum::WOR:
        os << "WOR";
        break;
    case Module::Default_nettypeEnum::UWIRE:
        os << "UWIRE";
        break;
    case Module::Default_nettypeEnum::SUPPLY0:
        os << "SUPPLY0";
        break;
    case Module::Default_nettypeEnum::SUPPLY1:
        os << "SUPPLY1";
        break;
    case Module::Default_nettypeEnum::NONE:
        os << "NONE";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse