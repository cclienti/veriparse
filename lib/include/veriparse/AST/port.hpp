// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_PORT_HPP
#define VERIPARSE_AST_PORT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

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

class Port : public Node
{
public:
    using Ptr = typename NodePointers<Port>::Ptr;
    using List = typename NodePointers<Port>::List;
    using ListPtr = typename NodePointers<Port>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    enum class DirectionEnum
    {
        NONE,
        INPUT,
        OUTPUT,
        INOUT,
        REF,
        CONST_REF
    };

    /**
     * Constructor, m_node_type is set to NodeType::Port.
     */
    Port(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Port.
     */
    Port(const Declaration::Ptr decl, const Node::Ptr expr, const std::string &name,
         const DirectionEnum &direction, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Port &operator=(const Port &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Port nodes are the same, do not check children.
     */
    virtual bool operator==(const Port &rhs) const;

    /**
     * Return true if the Port nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Port nodes are the same, do not check children.
     */
    virtual bool operator!=(const Port &rhs) const;

    /**
     * Return true if the Port nodes are the same, do not check children.
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
     * Return the child decl.
     */
    virtual Declaration::Ptr get_decl(void) const { return m_decl; }

    /**
     * Return the child expr.
     */
    virtual Node::Ptr get_expr(void) const { return m_expr; }

    /**
     * Change the child decl.
     */
    virtual void set_decl(Declaration::Ptr decl) { m_decl = decl; }

    /**
     * Change the child expr.
     */
    virtual void set_expr(Node::Ptr expr) { m_expr = expr; }

    /**
     * Return the property name.
     */
    virtual const std::string &get_name(void) const { return m_name; }

    /**
     * Return the property direction.
     */
    virtual const DirectionEnum &get_direction(void) const { return m_direction; }

    /**
     * Change the property name.
     */
    virtual void set_name(const std::string &name) { m_name = name; }

    /**
     * Change the property direction.
     */
    virtual void set_direction(const DirectionEnum &direction) { m_direction = direction; }

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

    Declaration::Ptr m_decl{};
    Node::Ptr m_expr{};
    std::string m_name{};
    DirectionEnum m_direction{};
};

std::ostream &operator<<(std::ostream &os, const Port &p);
std::ostream &operator<<(std::ostream &os, const Port::Ptr p);

std::ostream &operator<<(std::ostream &os, const Port::DirectionEnum p);

} // namespace AST
} // namespace Veriparse

#endif