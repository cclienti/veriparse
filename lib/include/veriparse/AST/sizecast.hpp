// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_SIZECAST_HPP
#define VERIPARSE_AST_SIZECAST_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/cast.hpp>

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

class SizeCast : public Cast
{
public:
    using Ptr = typename NodePointers<SizeCast>::Ptr;
    using List = typename NodePointers<SizeCast>::List;
    using ListPtr = typename NodePointers<SizeCast>::ListPtr;
    using Cast::operator=;
    using Cast::operator==;
    using Cast::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::SizeCast.
     */
    SizeCast(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::SizeCast.
     */
    SizeCast(const Node::Ptr size, const Node::Ptr expr, const std::string &filename = "",
             uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual SizeCast &operator=(const SizeCast &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the SizeCast nodes are the same, do not check children.
     */
    virtual bool operator==(const SizeCast &rhs) const;

    /**
     * Return true if the SizeCast nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the SizeCast nodes are the same, do not check children.
     */
    virtual bool operator!=(const SizeCast &rhs) const;

    /**
     * Return true if the SizeCast nodes are the same, do not check children.
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
     * Return the child size.
     */
    virtual Node::Ptr get_size(void) const { return m_size; }

    /**
     * Change the child size.
     */
    virtual void set_size(Node::Ptr size) { m_size = size; }

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

    Node::Ptr m_size{};
};

std::ostream &operator<<(std::ostream &os, const SizeCast &p);
std::ostream &operator<<(std::ostream &os, const SizeCast::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif