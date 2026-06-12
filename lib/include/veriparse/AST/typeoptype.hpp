// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_TYPEOPTYPE_HPP
#define VERIPARSE_AST_TYPEOPTYPE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/datatype.hpp>

#include <veriparse/AST/dimension.hpp>

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

class TypeOpType : public DataType
{
public:
    using Ptr = typename NodePointers<TypeOpType>::Ptr;
    using List = typename NodePointers<TypeOpType>::List;
    using ListPtr = typename NodePointers<TypeOpType>::ListPtr;
    using DataType::operator=;
    using DataType::operator==;
    using DataType::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::TypeOpType.
     */
    TypeOpType(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::TypeOpType.
     */
    TypeOpType(const DataType::Ptr type, const Dimension::ListPtr packed_dims,
               const DataType::SigningEnum &signing, const std::string &filename = "",
               uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual TypeOpType &operator=(const TypeOpType &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the TypeOpType nodes are the same, do not check children.
     */
    virtual bool operator==(const TypeOpType &rhs) const;

    /**
     * Return true if the TypeOpType nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the TypeOpType nodes are the same, do not check children.
     */
    virtual bool operator!=(const TypeOpType &rhs) const;

    /**
     * Return true if the TypeOpType nodes are the same, do not check children.
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
    virtual DataType::Ptr get_type(void) const { return m_type; }

    /**
     * Change the child type.
     */
    virtual void set_type(DataType::Ptr type) { m_type = type; }

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

    DataType::Ptr m_type{};
};

std::ostream &operator<<(std::ostream &os, const TypeOpType &p);
std::ostream &operator<<(std::ostream &os, const TypeOpType::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif