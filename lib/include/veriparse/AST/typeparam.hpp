// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_TYPEPARAM_HPP
#define VERIPARSE_AST_TYPEPARAM_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

#include <veriparse/AST/datatype.hpp>

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

class TypeParam : public Declaration
{
public:
    using Ptr = typename NodePointers<TypeParam>::Ptr;
    using List = typename NodePointers<TypeParam>::List;
    using ListPtr = typename NodePointers<TypeParam>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::TypeParam.
     */
    TypeParam(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::TypeParam.
     */
    TypeParam(const DataType::Ptr type, const bool &is_local, const std::string &name,
              const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual TypeParam &operator=(const TypeParam &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the TypeParam nodes are the same, do not check children.
     */
    virtual bool operator==(const TypeParam &rhs) const;

    /**
     * Return true if the TypeParam nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the TypeParam nodes are the same, do not check children.
     */
    virtual bool operator!=(const TypeParam &rhs) const;

    /**
     * Return true if the TypeParam nodes are the same, do not check children.
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

    bool m_is_local{};
};

std::ostream &operator<<(std::ostream &os, const TypeParam &p);
std::ostream &operator<<(std::ostream &os, const TypeParam::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif