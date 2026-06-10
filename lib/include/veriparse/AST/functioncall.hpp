// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_FUNCTIONCALL_HPP
#define VERIPARSE_AST_FUNCTIONCALL_HPP

#include <veriparse/AST/node.hpp>
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

class FunctionCall : public Node
{
public:
    using Ptr = typename NodePointers<FunctionCall>::Ptr;
    using List = typename NodePointers<FunctionCall>::List;
    using ListPtr = typename NodePointers<FunctionCall>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::FunctionCall.
     */
    FunctionCall(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::FunctionCall.
     */
    FunctionCall(const ScopeName::ListPtr scope, const Node::ListPtr args, const std::string &name,
                 const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual FunctionCall &operator=(const FunctionCall &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the FunctionCall nodes are the same, do not check children.
     */
    virtual bool operator==(const FunctionCall &rhs) const;

    /**
     * Return true if the FunctionCall nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the FunctionCall nodes are the same, do not check children.
     */
    virtual bool operator!=(const FunctionCall &rhs) const;

    /**
     * Return true if the FunctionCall nodes are the same, do not check children.
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
     * Return the child args.
     */
    virtual Node::ListPtr get_args(void) const { return m_args; }

    /**
     * Change the child scope.
     */
    virtual void set_scope(ScopeName::ListPtr scope) { m_scope = scope; }

    /**
     * Change the child args.
     */
    virtual void set_args(Node::ListPtr args) { m_args = args; }

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
    Node::ListPtr m_args{};
    std::string m_name{};
};

std::ostream &operator<<(std::ostream &os, const FunctionCall &p);
std::ostream &operator<<(std::ostream &os, const FunctionCall::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif