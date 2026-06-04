// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_STRUCTUNIONDEF_HPP
#define VERIPARSE_AST_STRUCTUNIONDEF_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/structmember.hpp>

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

class StructUnionDef : public Node
{
public:
    using Ptr = typename NodePointers<StructUnionDef>::Ptr;
    using List = typename NodePointers<StructUnionDef>::List;
    using ListPtr = typename NodePointers<StructUnionDef>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::StructUnionDef.
     */
    StructUnionDef(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::StructUnionDef.
     */
    StructUnionDef(const StructMember::ListPtr members, const bool &packed, const bool &sign,
                   const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual StructUnionDef &operator=(const StructUnionDef &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the StructUnionDef nodes are the same, do not check children.
     */
    virtual bool operator==(const StructUnionDef &rhs) const;

    /**
     * Return true if the StructUnionDef nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the StructUnionDef nodes are the same, do not check children.
     */
    virtual bool operator!=(const StructUnionDef &rhs) const;

    /**
     * Return true if the StructUnionDef nodes are the same, do not check children.
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
    virtual StructMember::ListPtr get_members(void) const { return m_members; }

    /**
     * Change the child members.
     */
    virtual void set_members(StructMember::ListPtr members) { m_members = members; }

    /**
     * Return the property packed.
     */
    virtual const bool &get_packed(void) const { return m_packed; }

    /**
     * Return the property sign.
     */
    virtual const bool &get_sign(void) const { return m_sign; }

    /**
     * Change the property packed.
     */
    virtual void set_packed(const bool &packed) { m_packed = packed; }

    /**
     * Change the property sign.
     */
    virtual void set_sign(const bool &sign) { m_sign = sign; }

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

    StructMember::ListPtr m_members;
    bool m_packed;
    bool m_sign;
};

std::ostream &operator<<(std::ostream &os, const StructUnionDef &p);
std::ostream &operator<<(std::ostream &os, const StructUnionDef::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif