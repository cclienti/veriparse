// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_MEMBER_HPP
#define VERIPARSE_AST_MEMBER_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

#include <veriparse/AST/datatype.hpp>
#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/rvalue.hpp>

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

class Member : public Declaration
{
public:
    using Ptr = typename NodePointers<Member>::Ptr;
    using List = typename NodePointers<Member>::List;
    using ListPtr = typename NodePointers<Member>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::Member.
     */
    Member(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Member.
     */
    Member(const Dimension::ListPtr unpacked_dims, const Rvalue::Ptr init, const DataType::Ptr type,
           const std::string &name, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Member &operator=(const Member &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Member nodes are the same, do not check children.
     */
    virtual bool operator==(const Member &rhs) const;

    /**
     * Return true if the Member nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Member nodes are the same, do not check children.
     */
    virtual bool operator!=(const Member &rhs) const;

    /**
     * Return true if the Member nodes are the same, do not check children.
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
     * Return the child unpacked_dims.
     */
    virtual Dimension::ListPtr get_unpacked_dims(void) const { return m_unpacked_dims; }

    /**
     * Return the child init.
     */
    virtual Rvalue::Ptr get_init(void) const { return m_init; }

    /**
     * Change the child unpacked_dims.
     */
    virtual void set_unpacked_dims(Dimension::ListPtr unpacked_dims)
    {
        m_unpacked_dims = unpacked_dims;
    }

    /**
     * Change the child init.
     */
    virtual void set_init(Rvalue::Ptr init) { m_init = init; }

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

    Dimension::ListPtr m_unpacked_dims{};
    Rvalue::Ptr m_init{};
};

std::ostream &operator<<(std::ostream &os, const Member &p);
std::ostream &operator<<(std::ostream &os, const Member::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif