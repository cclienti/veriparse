// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_TYPEDEF_HPP
#define VERIPARSE_AST_TYPEDEF_HPP

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

class Typedef : public Declaration
{
public:
    using Ptr = typename NodePointers<Typedef>::Ptr;
    using List = typename NodePointers<Typedef>::List;
    using ListPtr = typename NodePointers<Typedef>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    enum class Fwd_kindEnum
    {
        NONE,
        ENUM,
        STRUCT,
        UNION,
        CLASS,
        INTERFACE_CLASS
    };

    /**
     * Constructor, m_node_type is set to NodeType::Typedef.
     */
    Typedef(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Typedef.
     */
    Typedef(const Dimension::ListPtr unpacked_dims, const DataType::Ptr type,
            const Fwd_kindEnum &fwd_kind, const std::string &name, const std::string &filename = "",
            uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Typedef &operator=(const Typedef &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Typedef nodes are the same, do not check children.
     */
    virtual bool operator==(const Typedef &rhs) const;

    /**
     * Return true if the Typedef nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Typedef nodes are the same, do not check children.
     */
    virtual bool operator!=(const Typedef &rhs) const;

    /**
     * Return true if the Typedef nodes are the same, do not check children.
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
     * Change the child unpacked_dims.
     */
    virtual void set_unpacked_dims(Dimension::ListPtr unpacked_dims)
    {
        m_unpacked_dims = unpacked_dims;
    }

    /**
     * Return the property fwd_kind.
     */
    virtual const Fwd_kindEnum &get_fwd_kind(void) const { return m_fwd_kind; }

    /**
     * Change the property fwd_kind.
     */
    virtual void set_fwd_kind(const Fwd_kindEnum &fwd_kind) { m_fwd_kind = fwd_kind; }

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
    Fwd_kindEnum m_fwd_kind{};
};

std::ostream &operator<<(std::ostream &os, const Typedef &p);
std::ostream &operator<<(std::ostream &os, const Typedef::Ptr p);

std::ostream &operator<<(std::ostream &os, const Typedef::Fwd_kindEnum p);

} // namespace AST
} // namespace Veriparse

#endif