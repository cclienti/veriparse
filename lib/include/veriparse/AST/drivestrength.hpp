// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_DRIVESTRENGTH_HPP
#define VERIPARSE_AST_DRIVESTRENGTH_HPP

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

class DriveStrength : public Strength
{
public:
    using Ptr = typename NodePointers<DriveStrength>::Ptr;
    using List = typename NodePointers<DriveStrength>::List;
    using ListPtr = typename NodePointers<DriveStrength>::ListPtr;
    using Strength::operator=;
    using Strength::operator==;
    using Strength::operator!=;

    enum class S0Enum
    {
        SUPPLY0,
        STRONG0,
        PULL0,
        WEAK0,
        HIGHZ0
    };

    enum class S1Enum
    {
        SUPPLY1,
        STRONG1,
        PULL1,
        WEAK1,
        HIGHZ1
    };

    /**
     * Constructor, m_node_type is set to NodeType::DriveStrength.
     */
    DriveStrength(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::DriveStrength.
     */
    DriveStrength(const S0Enum &s0, const S1Enum &s1, const std::string &filename = "",
                  uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual DriveStrength &operator=(const DriveStrength &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the DriveStrength nodes are the same, do not check children.
     */
    virtual bool operator==(const DriveStrength &rhs) const;

    /**
     * Return true if the DriveStrength nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the DriveStrength nodes are the same, do not check children.
     */
    virtual bool operator!=(const DriveStrength &rhs) const;

    /**
     * Return true if the DriveStrength nodes are the same, do not check children.
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
     * Return the property s0.
     */
    virtual const S0Enum &get_s0(void) const { return m_s0; }

    /**
     * Return the property s1.
     */
    virtual const S1Enum &get_s1(void) const { return m_s1; }

    /**
     * Change the property s0.
     */
    virtual void set_s0(const S0Enum &s0) { m_s0 = s0; }

    /**
     * Change the property s1.
     */
    virtual void set_s1(const S1Enum &s1) { m_s1 = s1; }

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

    S0Enum m_s0{};
    S1Enum m_s1{};
};

std::ostream &operator<<(std::ostream &os, const DriveStrength &p);
std::ostream &operator<<(std::ostream &os, const DriveStrength::Ptr p);

std::ostream &operator<<(std::ostream &os, const DriveStrength::S0Enum p);

std::ostream &operator<<(std::ostream &os, const DriveStrength::S1Enum p);

} // namespace AST
} // namespace Veriparse

#endif