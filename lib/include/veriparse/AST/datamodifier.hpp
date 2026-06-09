// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_DATAMODIFIER_HPP
#define VERIPARSE_AST_DATAMODIFIER_HPP

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

class DataModifier : public Node
{
public:
    using Ptr = typename NodePointers<DataModifier>::Ptr;
    using List = typename NodePointers<DataModifier>::List;
    using ListPtr = typename NodePointers<DataModifier>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    enum class LifetimeEnum
    {
        NONE,
        AUTOMATIC,
        STATIC
    };

    /**
     * Constructor, m_node_type is set to NodeType::DataModifier.
     */
    DataModifier(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::DataModifier.
     */
    DataModifier(const Node::Ptr datatype, const bool &is_var, const bool &is_const,
                 const LifetimeEnum &lifetime, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual DataModifier &operator=(const DataModifier &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the DataModifier nodes are the same, do not check children.
     */
    virtual bool operator==(const DataModifier &rhs) const;

    /**
     * Return true if the DataModifier nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the DataModifier nodes are the same, do not check children.
     */
    virtual bool operator!=(const DataModifier &rhs) const;

    /**
     * Return true if the DataModifier nodes are the same, do not check children.
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
     * Return the child datatype.
     */
    virtual Node::Ptr get_datatype(void) const { return m_datatype; }

    /**
     * Change the child datatype.
     */
    virtual void set_datatype(Node::Ptr datatype) { m_datatype = datatype; }

    /**
     * Return the property is_var.
     */
    virtual const bool &get_is_var(void) const { return m_is_var; }

    /**
     * Return the property is_const.
     */
    virtual const bool &get_is_const(void) const { return m_is_const; }

    /**
     * Return the property lifetime.
     */
    virtual const LifetimeEnum &get_lifetime(void) const { return m_lifetime; }

    /**
     * Change the property is_var.
     */
    virtual void set_is_var(const bool &is_var) { m_is_var = is_var; }

    /**
     * Change the property is_const.
     */
    virtual void set_is_const(const bool &is_const) { m_is_const = is_const; }

    /**
     * Change the property lifetime.
     */
    virtual void set_lifetime(const LifetimeEnum &lifetime) { m_lifetime = lifetime; }

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

    Node::Ptr m_datatype{};
    bool m_is_var{};
    bool m_is_const{};
    LifetimeEnum m_lifetime{};
};

std::ostream &operator<<(std::ostream &os, const DataModifier &p);
std::ostream &operator<<(std::ostream &os, const DataModifier::Ptr p);

std::ostream &operator<<(std::ostream &os, const DataModifier::LifetimeEnum p);

} // namespace AST
} // namespace Veriparse

#endif