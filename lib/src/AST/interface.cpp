// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/interface.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse
{
namespace AST
{

Interface::Interface(const std::string &filename, uint32_t line) : Node(filename, line)
{
    set_node_type(NodeType::Interface);
    set_node_categories({NodeType::Node});
}

Interface::Interface(const Declaration::ListPtr params, const Port::ListPtr ports,
                     const Node::ListPtr items, const std::string &name,
                     const LifetimeEnum &lifetime, const Default_nettypeEnum &default_nettype,
                     const std::string &filename, uint32_t line)
    : Node(filename, line), m_params(params), m_ports(ports), m_items(items), m_name(name),
      m_lifetime(lifetime), m_default_nettype(default_nettype)
{
    set_node_type(NodeType::Interface);
    set_node_categories({NodeType::Node});
}

Interface &Interface::operator=(const Interface &rhs)
{
    Node::operator=(static_cast<const Node &>(rhs));
    set_name(rhs.get_name());
    set_lifetime(rhs.get_lifetime());
    set_default_nettype(rhs.get_default_nettype());
    return *this;
}

Node &Interface::operator=(const Node &rhs)
{
    const Interface &rhs_cast = static_cast<const Interface &>(rhs);
    return static_cast<Node &>(operator=(rhs_cast));
}

bool Interface::operator==(const Interface &rhs) const
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

bool Interface::operator==(const Node &rhs) const
{
    const Interface &rhs_cast = static_cast<const Interface &>(rhs);
    return operator==(rhs_cast);
}

bool Interface::operator!=(const Interface &rhs) const { return !(operator==(rhs)); }

bool Interface::operator!=(const Node &rhs) const { return !(operator==(rhs)); }

bool Interface::remove(Node::Ptr node) { return replace(node, AST::Node::Ptr(nullptr)); }

bool Interface::replace(Node::Ptr node, Node::Ptr new_node)
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
                        LOG_WARNING << *this << ", "
                                    << "Interface::replace matches multiple times "
                                       "(list(Declaration)::params)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Declaration>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Interface::replace "
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
                        LOG_WARNING
                            << *this << ", "
                            << "Interface::replace matches multiple times (list(Port)::ports)";
                    }
                    if(new_node) {
                        new_list->push_back(cast_to<Port>(new_node));
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Interface::replace "
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
                        LOG_WARNING
                            << *this << ", "
                            << "Interface::replace matches multiple times (list(Node)::items)";
                    }
                    if(new_node) {
                        new_list->push_back(new_node);
                    }
                    found = true;
                }
            } else {
                LOG_WARNING << *this << ", "
                            << "found an empty node during Interface::replace "
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

bool Interface::replace(Node::Ptr node, Node::ListPtr new_nodes)
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
                        LOG_WARNING << *this << ", "
                                    << "Interface::replace matches multiple times "
                                       "(list(Declaration)::params)";
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
                            << "found an empty node during Interface::replace "
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
                        LOG_WARNING
                            << *this << ", "
                            << "Interface::replace matches multiple times (list(Port)::ports)";
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
                            << "found an empty node during Interface::replace "
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
                        LOG_WARNING
                            << *this << ", "
                            << "Interface::replace matches multiple times (list(Node)::items)";
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
                            << "found an empty node during Interface::replace "
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

Interface::ListPtr Interface::clone_list(const ListPtr nodes)
{
    ListPtr list;
    if(nodes) {
        list = std::make_shared<List>();
        for(const Ptr &p : *nodes) {
            list->push_back(cast_to<Interface>(p->clone()));
        }
    }
    return list;
}

Node::ListPtr Interface::get_children(void) const
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

void Interface::clone_children(Node::Ptr new_node) const
{
    cast_to<Interface>(new_node)->set_params(Declaration::clone_list(get_params()));
    cast_to<Interface>(new_node)->set_ports(Port::clone_list(get_ports()));
    cast_to<Interface>(new_node)->set_items(Node::clone_list(get_items()));
}

Node::Ptr Interface::alloc_same(void) const
{
    Ptr p(new Interface);
    return p;
}

std::ostream &operator<<(std::ostream &os, const Interface &p)
{
    os << "Interface: {";
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

std::ostream &operator<<(std::ostream &os, const Interface::Ptr p)
{
    if(p) {
        os << *p;
    } else {
        os << "Interface: {nullptr}";
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const Interface::LifetimeEnum p)
{
    switch(p) {
    case Interface::LifetimeEnum::NONE:
        os << "NONE";
        break;
    case Interface::LifetimeEnum::AUTOMATIC:
        os << "AUTOMATIC";
        break;
    case Interface::LifetimeEnum::STATIC:
        os << "STATIC";
        break;
    default:
        break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Interface::Default_nettypeEnum p)
{
    switch(p) {
    case Interface::Default_nettypeEnum::WIRE:
        os << "WIRE";
        break;
    case Interface::Default_nettypeEnum::TRI:
        os << "TRI";
        break;
    case Interface::Default_nettypeEnum::TRI0:
        os << "TRI0";
        break;
    case Interface::Default_nettypeEnum::TRI1:
        os << "TRI1";
        break;
    case Interface::Default_nettypeEnum::TRIAND:
        os << "TRIAND";
        break;
    case Interface::Default_nettypeEnum::TRIOR:
        os << "TRIOR";
        break;
    case Interface::Default_nettypeEnum::TRIREG:
        os << "TRIREG";
        break;
    case Interface::Default_nettypeEnum::WAND:
        os << "WAND";
        break;
    case Interface::Default_nettypeEnum::WOR:
        os << "WOR";
        break;
    case Interface::Default_nettypeEnum::UWIRE:
        os << "UWIRE";
        break;
    case Interface::Default_nettypeEnum::SUPPLY0:
        os << "SUPPLY0";
        break;
    case Interface::Default_nettypeEnum::SUPPLY1:
        os << "SUPPLY1";
        break;
    case Interface::Default_nettypeEnum::NONE:
        os << "NONE";
        break;
    default:
        break;
    }
    return os;
}

} // namespace AST
} // namespace Veriparse