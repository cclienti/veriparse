// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_PATTERNITEM_HPP
#define VERIPARSE_AST_PATTERNITEM_HPP

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

class PatternItem : public Node
{
public:
    using Ptr = typename NodePointers<PatternItem>::Ptr;
    using List = typename NodePointers<PatternItem>::List;
    using ListPtr = typename NodePointers<PatternItem>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::PatternItem.
     */
    PatternItem(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::PatternItem.
     */
    PatternItem(const Node::Ptr key, const Node::Ptr value, const bool &is_default,
                const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual PatternItem &operator=(const PatternItem &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the PatternItem nodes are the same, do not check children.
     */
    virtual bool operator==(const PatternItem &rhs) const;

    /**
     * Return true if the PatternItem nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the PatternItem nodes are the same, do not check children.
     */
    virtual bool operator!=(const PatternItem &rhs) const;

    /**
     * Return true if the PatternItem nodes are the same, do not check children.
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
     * Return the child key.
     */
    virtual Node::Ptr get_key(void) const { return m_key; }

    /**
     * Return the child value.
     */
    virtual Node::Ptr get_value(void) const { return m_value; }

    /**
     * Change the child key.
     */
    virtual void set_key(Node::Ptr key) { m_key = key; }

    /**
     * Change the child value.
     */
    virtual void set_value(Node::Ptr value) { m_value = value; }

    /**
     * Return the property is_default.
     */
    virtual const bool &get_is_default(void) const { return m_is_default; }

    /**
     * Change the property is_default.
     */
    virtual void set_is_default(const bool &is_default) { m_is_default = is_default; }

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

    Node::Ptr m_key{};
    Node::Ptr m_value{};
    bool m_is_default{};
};

std::ostream &operator<<(std::ostream &os, const PatternItem &p);
std::ostream &operator<<(std::ostream &os, const PatternItem::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif