// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_UNIONTYPE_HPP
#define VERIPARSE_AST_UNIONTYPE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/datatype.hpp>

#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/member.hpp>

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

class UnionType : public DataType
{
public:
    using Ptr = typename NodePointers<UnionType>::Ptr;
    using List = typename NodePointers<UnionType>::List;
    using ListPtr = typename NodePointers<UnionType>::ListPtr;
    using DataType::operator=;
    using DataType::operator==;
    using DataType::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::UnionType.
     */
    UnionType(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::UnionType.
     */
    UnionType(const Member::ListPtr members, const Dimension::ListPtr packed_dims,
              const bool &is_packed, const bool &is_tagged, const DataType::SigningEnum &signing,
              const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual UnionType &operator=(const UnionType &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the UnionType nodes are the same, do not check children.
     */
    virtual bool operator==(const UnionType &rhs) const;

    /**
     * Return true if the UnionType nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the UnionType nodes are the same, do not check children.
     */
    virtual bool operator!=(const UnionType &rhs) const;

    /**
     * Return true if the UnionType nodes are the same, do not check children.
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
     * Return the child members.
     */
    virtual Member::ListPtr get_members(void) const { return m_members; }

    /**
     * Change the child members.
     */
    virtual void set_members(Member::ListPtr members) { m_members = members; }

    /**
     * Return the property is_packed.
     */
    virtual const bool &get_is_packed(void) const { return m_is_packed; }

    /**
     * Return the property is_tagged.
     */
    virtual const bool &get_is_tagged(void) const { return m_is_tagged; }

    /**
     * Change the property is_packed.
     */
    virtual void set_is_packed(const bool &is_packed) { m_is_packed = is_packed; }

    /**
     * Change the property is_tagged.
     */
    virtual void set_is_tagged(const bool &is_tagged) { m_is_tagged = is_tagged; }

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

    Member::ListPtr m_members{};
    bool m_is_packed{};
    bool m_is_tagged{};
};

std::ostream &operator<<(std::ostream &os, const UnionType &p);
std::ostream &operator<<(std::ostream &os, const UnionType::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif