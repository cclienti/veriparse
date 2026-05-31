// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_PACKAGE_HPP
#define VERIPARSE_AST_PACKAGE_HPP

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

class Package : public Node
{
public:
    using Ptr = typename NodePointers<Package>::Ptr;
    using List = typename NodePointers<Package>::List;
    using ListPtr = typename NodePointers<Package>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::Package.
     */
    Package(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Package.
     */
    Package(const Node::ListPtr items, const std::string &name, const std::string &filename = "",
            uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Package &operator=(const Package &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Package nodes are the same, do not check children.
     */
    virtual bool operator==(const Package &rhs) const;

    /**
     * Return true if the Package nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Package nodes are the same, do not check children.
     */
    virtual bool operator!=(const Package &rhs) const;

    /**
     * Return true if the Package nodes are the same, do not check children.
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
     * Return the child items.
     */
    virtual Node::ListPtr get_items(void) const { return m_items; }

    /**
     * Change the child items.
     */
    virtual void set_items(Node::ListPtr items) { m_items = items; }

    /**
     * Return the property name.
     */
    virtual const std::string &get_name(void) const { return m_name; }

    /**
     * Change the property name.
     */
    virtual void set_name(const std::string &name) { m_name = name; }

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

    Node::ListPtr m_items;
    std::string m_name;
};

std::ostream &operator<<(std::ostream &os, const Package &p);
std::ostream &operator<<(std::ostream &os, const Package::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif