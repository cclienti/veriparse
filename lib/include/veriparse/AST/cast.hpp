// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_CAST_HPP
#define VERIPARSE_AST_CAST_HPP

#include <veriparse/AST/node.hpp>

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

class Cast : public Node
{
public:
    using Ptr = typename NodePointers<Cast>::Ptr;
    using List = typename NodePointers<Cast>::List;
    using ListPtr = typename NodePointers<Cast>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::Cast.
     */
    Cast(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Cast.
     */
    Cast(const Node::Ptr type, const Node::Ptr expr, const std::string &filename = "",
         uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Cast &operator=(const Cast &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Cast nodes are the same, do not check children.
     */
    virtual bool operator==(const Cast &rhs) const;

    /**
     * Return true if the Cast nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Cast nodes are the same, do not check children.
     */
    virtual bool operator!=(const Cast &rhs) const;

    /**
     * Return true if the Cast nodes are the same, do not check children.
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
     * Return the child type.
     */
    virtual Node::Ptr get_type(void) const { return m_type; }

    /**
     * Return the child expr.
     */
    virtual Node::Ptr get_expr(void) const { return m_expr; }

    /**
     * Change the child type.
     */
    virtual void set_type(Node::Ptr type) { m_type = type; }

    /**
     * Change the child expr.
     */
    virtual void set_expr(Node::Ptr expr) { m_expr = expr; }

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

    Node::Ptr m_type{};
    Node::Ptr m_expr{};
};

std::ostream &operator<<(std::ostream &os, const Cast &p);
std::ostream &operator<<(std::ostream &os, const Cast::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif