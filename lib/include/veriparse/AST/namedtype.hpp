// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NAMEDTYPE_HPP
#define VERIPARSE_AST_NAMEDTYPE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/datatype.hpp>

#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/scopename.hpp>

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

class NamedType : public DataType
{
public:
    using Ptr = typename NodePointers<NamedType>::Ptr;
    using List = typename NodePointers<NamedType>::List;
    using ListPtr = typename NodePointers<NamedType>::ListPtr;
    using DataType::operator=;
    using DataType::operator==;
    using DataType::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::NamedType.
     */
    NamedType(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::NamedType.
     */
    NamedType(const ScopeName::ListPtr scope, const Dimension::ListPtr packed_dims,
              const std::string &name, const DataType::SigningEnum &signing,
              const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual NamedType &operator=(const NamedType &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the NamedType nodes are the same, do not check children.
     */
    virtual bool operator==(const NamedType &rhs) const;

    /**
     * Return true if the NamedType nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the NamedType nodes are the same, do not check children.
     */
    virtual bool operator!=(const NamedType &rhs) const;

    /**
     * Return true if the NamedType nodes are the same, do not check children.
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
     * Return the child scope.
     */
    virtual ScopeName::ListPtr get_scope(void) const { return m_scope; }

    /**
     * Change the child scope.
     */
    virtual void set_scope(ScopeName::ListPtr scope) { m_scope = scope; }

    /**
     * Return the property name.
     */
    virtual const std::string &get_name(void) const { return m_name; }

    /**
     * Change the property name.
     */
    virtual void set_name(const std::string &name) { m_name = name; }

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

    ScopeName::ListPtr m_scope{};
    std::string m_name{};
};

std::ostream &operator<<(std::ostream &os, const NamedType &p);
std::ostream &operator<<(std::ostream &os, const NamedType::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif