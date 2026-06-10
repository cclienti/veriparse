// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_DATATYPE_HPP
#define VERIPARSE_AST_DATATYPE_HPP

#include <veriparse/AST/node.hpp>
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

class DataType : public Node
{
public:
    using Ptr = typename NodePointers<DataType>::Ptr;
    using List = typename NodePointers<DataType>::List;
    using ListPtr = typename NodePointers<DataType>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    enum class SigningEnum
    {
        NONE,
        SIGNED,
        UNSIGNED
    };

    /**
     * Constructor, m_node_type is set to NodeType::DataType.
     */
    DataType(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::DataType.
     */
    DataType(const Dimension::ListPtr packed_dims, const SigningEnum &signing,
             const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual DataType &operator=(const DataType &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the DataType nodes are the same, do not check children.
     */
    virtual bool operator==(const DataType &rhs) const;

    /**
     * Return true if the DataType nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the DataType nodes are the same, do not check children.
     */
    virtual bool operator!=(const DataType &rhs) const;

    /**
     * Return true if the DataType nodes are the same, do not check children.
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
     * Return the child packed_dims.
     */
    virtual Dimension::ListPtr get_packed_dims(void) const { return m_packed_dims; }

    /**
     * Change the child packed_dims.
     */
    virtual void set_packed_dims(Dimension::ListPtr packed_dims) { m_packed_dims = packed_dims; }

    /**
     * Return the property signing.
     */
    virtual const SigningEnum &get_signing(void) const { return m_signing; }

    /**
     * Change the property signing.
     */
    virtual void set_signing(const SigningEnum &signing) { m_signing = signing; }

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

    Dimension::ListPtr m_packed_dims{};
    SigningEnum m_signing{};
};

std::ostream &operator<<(std::ostream &os, const DataType &p);
std::ostream &operator<<(std::ostream &os, const DataType::Ptr p);

std::ostream &operator<<(std::ostream &os, const DataType::SigningEnum p);

} // namespace AST
} // namespace Veriparse

#endif