// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_PARAM_HPP
#define VERIPARSE_AST_PARAM_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

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

class Param : public Declaration
{
public:
    using Ptr = typename NodePointers<Param>::Ptr;
    using List = typename NodePointers<Param>::List;
    using ListPtr = typename NodePointers<Param>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::Param.
     */
    Param(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Param.
     */
    Param(const Node::Ptr value, const Dimension::ListPtr unpacked_dims, const DataType::Ptr type,
          const bool &is_local, const std::string &name, const std::string &filename = "",
          uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Param &operator=(const Param &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Param nodes are the same, do not check children.
     */
    virtual bool operator==(const Param &rhs) const;

    /**
     * Return true if the Param nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Param nodes are the same, do not check children.
     */
    virtual bool operator!=(const Param &rhs) const;

    /**
     * Return true if the Param nodes are the same, do not check children.
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
     * Return the child value.
     */
    virtual Node::Ptr get_value(void) const { return m_value; }

    /**
     * Return the child unpacked_dims.
     */
    virtual Dimension::ListPtr get_unpacked_dims(void) const { return m_unpacked_dims; }

    /**
     * Change the child value.
     */
    virtual void set_value(Node::Ptr value) { m_value = value; }

    /**
     * Change the child unpacked_dims.
     */
    virtual void set_unpacked_dims(Dimension::ListPtr unpacked_dims)
    {
        m_unpacked_dims = unpacked_dims;
    }

    /**
     * Return the property is_local.
     */
    virtual const bool &get_is_local(void) const { return m_is_local; }

    /**
     * Change the property is_local.
     */
    virtual void set_is_local(const bool &is_local) { m_is_local = is_local; }

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

    Node::Ptr m_value{};
    Dimension::ListPtr m_unpacked_dims{};
    bool m_is_local{};
};

std::ostream &operator<<(std::ostream &os, const Param &p);
std::ostream &operator<<(std::ostream &os, const Param::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif