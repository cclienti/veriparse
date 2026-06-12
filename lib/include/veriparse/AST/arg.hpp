// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_ARG_HPP
#define VERIPARSE_AST_ARG_HPP

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

class Arg : public Declaration
{
public:
    using Ptr = typename NodePointers<Arg>::Ptr;
    using List = typename NodePointers<Arg>::List;
    using ListPtr = typename NodePointers<Arg>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    enum class DirectionEnum
    {
        NONE,
        INPUT,
        OUTPUT,
        INOUT,
        REF,
        CONST_REF
    };

    /**
     * Constructor, m_node_type is set to NodeType::Arg.
     */
    Arg(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Arg.
     */
    Arg(const Dimension::ListPtr unpacked_dims, const Node::Ptr default_value,
        const DataType::Ptr type, const bool &is_var, const DirectionEnum &direction,
        const std::string &name, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Arg &operator=(const Arg &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Arg nodes are the same, do not check children.
     */
    virtual bool operator==(const Arg &rhs) const;

    /**
     * Return true if the Arg nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Arg nodes are the same, do not check children.
     */
    virtual bool operator!=(const Arg &rhs) const;

    /**
     * Return true if the Arg nodes are the same, do not check children.
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
     * Return the child default_value.
     */
    virtual Node::Ptr get_default_value(void) const { return m_default_value; }

    /**
     * Change the child unpacked_dims.
     */
    virtual void set_unpacked_dims(Dimension::ListPtr unpacked_dims)
    {
        m_unpacked_dims = unpacked_dims;
    }

    /**
     * Change the child default_value.
     */
    virtual void set_default_value(Node::Ptr default_value) { m_default_value = default_value; }

    /**
     * Return the property is_var.
     */
    virtual const bool &get_is_var(void) const { return m_is_var; }

    /**
     * Return the property direction.
     */
    virtual const DirectionEnum &get_direction(void) const { return m_direction; }

    /**
     * Change the property is_var.
     */
    virtual void set_is_var(const bool &is_var) { m_is_var = is_var; }

    /**
     * Change the property direction.
     */
    virtual void set_direction(const DirectionEnum &direction) { m_direction = direction; }

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
    Node::Ptr m_default_value{};
    bool m_is_var{};
    DirectionEnum m_direction{};
};

std::ostream &operator<<(std::ostream &os, const Arg &p);
std::ostream &operator<<(std::ostream &os, const Arg::Ptr p);

std::ostream &operator<<(std::ostream &os, const Arg::DirectionEnum p);

} // namespace AST
} // namespace Veriparse

#endif