// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_INTERFACE_HPP
#define VERIPARSE_AST_INTERFACE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>
#include <veriparse/AST/port.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse
{
namespace AST
{

class Interface : public Node
{
public:
    using Ptr = typename NodePointers<Interface>::Ptr;
    using List = typename NodePointers<Interface>::List;
    using ListPtr = typename NodePointers<Interface>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    enum class LifetimeEnum
    {
        NONE,
        AUTOMATIC,
        STATIC
    };

    enum class Default_nettypeEnum
    {
        WIRE,
        TRI,
        TRI0,
        TRI1,
        TRIAND,
        TRIOR,
        TRIREG,
        WAND,
        WOR,
        UWIRE,
        SUPPLY0,
        SUPPLY1,
        NONE
    };

    /**
     * Constructor, m_node_type is set to NodeType::Interface.
     */
    Interface(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Interface.
     */
    Interface(const Declaration::ListPtr params, const Port::ListPtr ports,
              const Node::ListPtr items, const std::string &name, const LifetimeEnum &lifetime,
              const Default_nettypeEnum &default_nettype, const std::string &filename = "",
              uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Interface &operator=(const Interface &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Interface nodes are the same, do not check children.
     */
    virtual bool operator==(const Interface &rhs) const;

    /**
     * Return true if the Interface nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Interface nodes are the same, do not check children.
     */
    virtual bool operator!=(const Interface &rhs) const;

    /**
     * Return true if the Interface nodes are the same, do not check children.
     */
    virtual bool operator!=(const Node &rhs) const override;

    /**
     * Remove the given child.
     */
    virtual bool remove(Node::Ptr node) override;

    /**
     * Replace the given children node by the new node.
     */
    virtual bool replace(Node::Ptr node, Node::Ptr new_node) override;

    /**
     * Replace the given children node by the new nodes in the list.
     */
    virtual bool replace(Node::Ptr node, Node::ListPtr new_nodes) override;

    /**
     * Return the child params.
     */
    virtual Declaration::ListPtr get_params(void) const { return m_params; }

    /**
     * Return the child ports.
     */
    virtual Port::ListPtr get_ports(void) const { return m_ports; }

    /**
     * Return the child items.
     */
    virtual Node::ListPtr get_items(void) const { return m_items; }

    /**
     * Change the child params.
     */
    virtual void set_params(Declaration::ListPtr params) { m_params = params; }

    /**
     * Change the child ports.
     */
    virtual void set_ports(Port::ListPtr ports) { m_ports = ports; }

    /**
     * Change the child items.
     */
    virtual void set_items(Node::ListPtr items) { m_items = items; }

    /**
     * Return the property name.
     */
    virtual const std::string &get_name(void) const { return m_name; }

    /**
     * Return the property lifetime.
     */
    virtual const LifetimeEnum &get_lifetime(void) const { return m_lifetime; }

    /**
     * Return the property default_nettype.
     */
    virtual const Default_nettypeEnum &get_default_nettype(void) const { return m_default_nettype; }

    /**
     * Change the property name.
     */
    virtual void set_name(const std::string &name) { m_name = name; }

    /**
     * Change the property lifetime.
     */
    virtual void set_lifetime(const LifetimeEnum &lifetime) { m_lifetime = lifetime; }

    /**
     * Change the property default_nettype.
     */
    virtual void set_default_nettype(const Default_nettypeEnum &default_nettype)
    {
        m_default_nettype = default_nettype;
    }

    /**
     * Return the children list using the private children member
     * pointers.
     */
    virtual Node::ListPtr get_children(void) const override;

    /**
     * Return a clone of the current List and of all children (recursive).
     */
    static ListPtr clone_list(const ListPtr nodes);

protected:
    /**
     * Clone the instance children and attach them to the
     * new_node. This method is called by Node::clone().
     */
    virtual void clone_children(Node::Ptr new_node) const override;

private:
    /**
     * Allocate a new node with the same node type than the current instance.
     */
    virtual Node::Ptr alloc_same(void) const override;

    Declaration::ListPtr m_params{};
    Port::ListPtr m_ports{};
    Node::ListPtr m_items{};
    std::string m_name{};
    LifetimeEnum m_lifetime{};
    Default_nettypeEnum m_default_nettype{};
};

std::ostream &operator<<(std::ostream &os, const Interface &p);
std::ostream &operator<<(std::ostream &os, const Interface::Ptr p);

std::ostream &operator<<(std::ostream &os, const Interface::LifetimeEnum p);

std::ostream &operator<<(std::ostream &os, const Interface::Default_nettypeEnum p);

} // namespace AST
} // namespace Veriparse

#endif