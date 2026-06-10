// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NET_HPP
#define VERIPARSE_AST_NET_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

#include <veriparse/AST/datatype.hpp>
#include <veriparse/AST/delaystatement.hpp>
#include <veriparse/AST/dimension.hpp>
#include <veriparse/AST/rvalue.hpp>
#include <veriparse/AST/strength.hpp>

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

class Net : public Declaration
{
public:
    using Ptr = typename NodePointers<Net>::Ptr;
    using List = typename NodePointers<Net>::List;
    using ListPtr = typename NodePointers<Net>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::Net.
     */
    Net(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::Net.
     */
    Net(const Dimension::ListPtr unpacked_dims, const Rvalue::Ptr cont_assign,
        const Strength::Ptr strength, const DelayStatement::Ptr ldelay,
        const DelayStatement::Ptr rdelay, const DataType::Ptr type, const bool &is_vectored,
        const bool &is_scalared, const std::string &name, const std::string &filename = "",
        uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Net &operator=(const Net &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the Net nodes are the same, do not check children.
     */
    virtual bool operator==(const Net &rhs) const;

    /**
     * Return true if the Net nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the Net nodes are the same, do not check children.
     */
    virtual bool operator!=(const Net &rhs) const;

    /**
     * Return true if the Net nodes are the same, do not check children.
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
     * Return the child cont_assign.
     */
    virtual Rvalue::Ptr get_cont_assign(void) const { return m_cont_assign; }

    /**
     * Return the child strength.
     */
    virtual Strength::Ptr get_strength(void) const { return m_strength; }

    /**
     * Return the child ldelay.
     */
    virtual DelayStatement::Ptr get_ldelay(void) const { return m_ldelay; }

    /**
     * Return the child rdelay.
     */
    virtual DelayStatement::Ptr get_rdelay(void) const { return m_rdelay; }

    /**
     * Change the child unpacked_dims.
     */
    virtual void set_unpacked_dims(Dimension::ListPtr unpacked_dims)
    {
        m_unpacked_dims = unpacked_dims;
    }

    /**
     * Change the child cont_assign.
     */
    virtual void set_cont_assign(Rvalue::Ptr cont_assign) { m_cont_assign = cont_assign; }

    /**
     * Change the child strength.
     */
    virtual void set_strength(Strength::Ptr strength) { m_strength = strength; }

    /**
     * Change the child ldelay.
     */
    virtual void set_ldelay(DelayStatement::Ptr ldelay) { m_ldelay = ldelay; }

    /**
     * Change the child rdelay.
     */
    virtual void set_rdelay(DelayStatement::Ptr rdelay) { m_rdelay = rdelay; }

    /**
     * Return the property is_vectored.
     */
    virtual const bool &get_is_vectored(void) const { return m_is_vectored; }

    /**
     * Return the property is_scalared.
     */
    virtual const bool &get_is_scalared(void) const { return m_is_scalared; }

    /**
     * Change the property is_vectored.
     */
    virtual void set_is_vectored(const bool &is_vectored) { m_is_vectored = is_vectored; }

    /**
     * Change the property is_scalared.
     */
    virtual void set_is_scalared(const bool &is_scalared) { m_is_scalared = is_scalared; }

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
    Rvalue::Ptr m_cont_assign{};
    Strength::Ptr m_strength{};
    DelayStatement::Ptr m_ldelay{};
    DelayStatement::Ptr m_rdelay{};
    bool m_is_vectored{};
    bool m_is_scalared{};
};

std::ostream &operator<<(std::ostream &os, const Net &p);
std::ostream &operator<<(std::ostream &os, const Net::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif