// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_INTERFACETYPE_HPP
#define VERIPARSE_AST_INTERFACETYPE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/datatype.hpp>

#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/paramarg.hpp>

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

class InterfaceType : public DataType
{
public:
    using Ptr = typename NodePointers<InterfaceType>::Ptr;
    using List = typename NodePointers<InterfaceType>::List;
    using ListPtr = typename NodePointers<InterfaceType>::ListPtr;
    using DataType::operator=;
    using DataType::operator==;
    using DataType::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::InterfaceType.
     */
    InterfaceType(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::InterfaceType.
     */
    InterfaceType(const ParamArg::ListPtr params, const Dimension::ListPtr packed_dims,
                  const std::string &name, const std::string &modport, const bool &is_virtual,
                  const DataType::SigningEnum &signing, const std::string &filename = "",
                  uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual InterfaceType &operator=(const InterfaceType &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the InterfaceType nodes are the same, do not check children.
     */
    virtual bool operator==(const InterfaceType &rhs) const;

    /**
     * Return true if the InterfaceType nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the InterfaceType nodes are the same, do not check children.
     */
    virtual bool operator!=(const InterfaceType &rhs) const;

    /**
     * Return true if the InterfaceType nodes are the same, do not check children.
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
     * Return the child params.
     */
    virtual ParamArg::ListPtr get_params(void) const { return m_params; }

    /**
     * Change the child params.
     */
    virtual void set_params(ParamArg::ListPtr params) { m_params = params; }

    /**
     * Return the property name.
     */
    virtual const std::string &get_name(void) const { return m_name; }

    /**
     * Return the property modport.
     */
    virtual const std::string &get_modport(void) const { return m_modport; }

    /**
     * Return the property is_virtual.
     */
    virtual const bool &get_is_virtual(void) const { return m_is_virtual; }

    /**
     * Change the property name.
     */
    virtual void set_name(const std::string &name) { m_name = name; }

    /**
     * Change the property modport.
     */
    virtual void set_modport(const std::string &modport) { m_modport = modport; }

    /**
     * Change the property is_virtual.
     */
    virtual void set_is_virtual(const bool &is_virtual) { m_is_virtual = is_virtual; }

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

    ParamArg::ListPtr m_params{};
    std::string m_name{};
    std::string m_modport{};
    bool m_is_virtual{};
};

std::ostream &operator<<(std::ostream &os, const InterfaceType &p);
std::ostream &operator<<(std::ostream &os, const InterfaceType::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif