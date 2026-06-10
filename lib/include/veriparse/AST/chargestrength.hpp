// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_CHARGESTRENGTH_HPP
#define VERIPARSE_AST_CHARGESTRENGTH_HPP

#include <veriparse/AST/node.hpp>
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

class ChargeStrength : public Strength
{
public:
    using Ptr = typename NodePointers<ChargeStrength>::Ptr;
    using List = typename NodePointers<ChargeStrength>::List;
    using ListPtr = typename NodePointers<ChargeStrength>::ListPtr;
    using Strength::operator=;
    using Strength::operator==;
    using Strength::operator!=;

    enum class ChargeEnum
    {
        SMALL,
        MEDIUM,
        LARGE
    };

    /**
     * Constructor, m_node_type is set to NodeType::ChargeStrength.
     */
    ChargeStrength(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::ChargeStrength.
     */
    ChargeStrength(const ChargeEnum &charge, const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual ChargeStrength &operator=(const ChargeStrength &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the ChargeStrength nodes are the same, do not check children.
     */
    virtual bool operator==(const ChargeStrength &rhs) const;

    /**
     * Return true if the ChargeStrength nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the ChargeStrength nodes are the same, do not check children.
     */
    virtual bool operator!=(const ChargeStrength &rhs) const;

    /**
     * Return true if the ChargeStrength nodes are the same, do not check children.
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
     * Return the property charge.
     */
    virtual const ChargeEnum &get_charge(void) const { return m_charge; }

    /**
     * Change the property charge.
     */
    virtual void set_charge(const ChargeEnum &charge) { m_charge = charge; }

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

    ChargeEnum m_charge{};
};

std::ostream &operator<<(std::ostream &os, const ChargeStrength &p);
std::ostream &operator<<(std::ostream &os, const ChargeStrength::Ptr p);

std::ostream &operator<<(std::ostream &os, const ChargeStrength::ChargeEnum p);

} // namespace AST
} // namespace Veriparse

#endif