// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_INTERFACEINSTANCE_HPP
#define VERIPARSE_AST_INTERFACEINSTANCE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/instance.hpp>

#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/paramarg.hpp>
#include <veriparse/AST/portarg.hpp>

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

class InterfaceInstance : public Instance
{
public:
    using Ptr = typename NodePointers<InterfaceInstance>::Ptr;
    using List = typename NodePointers<InterfaceInstance>::List;
    using ListPtr = typename NodePointers<InterfaceInstance>::ListPtr;
    using Instance::operator=;
    using Instance::operator==;
    using Instance::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::InterfaceInstance.
     */
    InterfaceInstance(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::InterfaceInstance.
     */
    InterfaceInstance(const Dimension::Ptr array, const ParamArg::ListPtr parameterlist,
                      const PortArg::ListPtr portlist, const std::string &module,
                      const std::string &name, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual InterfaceInstance &operator=(const InterfaceInstance &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the InterfaceInstance nodes are the same, do not check children.
     */
    virtual bool operator==(const InterfaceInstance &rhs) const;

    /**
     * Return true if the InterfaceInstance nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the InterfaceInstance nodes are the same, do not check children.
     */
    virtual bool operator!=(const InterfaceInstance &rhs) const;

    /**
     * Return true if the InterfaceInstance nodes are the same, do not check children.
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
};

std::ostream &operator<<(std::ostream &os, const InterfaceInstance &p);
std::ostream &operator<<(std::ostream &os, const InterfaceInstance::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif