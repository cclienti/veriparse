// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_ENUMDEF_HPP
#define VERIPARSE_AST_ENUMDEF_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/enumitem.hpp>
#include <veriparse/AST/width.hpp>

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

class EnumDef : public Node
{
public:
    using Ptr = typename NodePointers<EnumDef>::Ptr;
    using List = typename NodePointers<EnumDef>::List;
    using ListPtr = typename NodePointers<EnumDef>::ListPtr;
    using Node::operator=;
    using Node::operator==;
    using Node::operator!=;

    enum class Base_typeEnum
    {
        LOGIC,
        BIT,
        INT,
        NONE
    };

    /**
     * Constructor, m_node_type is set to NodeType::EnumDef.
     */
    EnumDef(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::EnumDef.
     */
    EnumDef(const Width::ListPtr widths, const EnumItem::ListPtr items,
            const Base_typeEnum &base_type, const bool &sign, const std::string &filename = "",
            uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual EnumDef &operator=(const EnumDef &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the EnumDef nodes are the same, do not check children.
     */
    virtual bool operator==(const EnumDef &rhs) const;

    /**
     * Return true if the EnumDef nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the EnumDef nodes are the same, do not check children.
     */
    virtual bool operator!=(const EnumDef &rhs) const;

    /**
     * Return true if the EnumDef nodes are the same, do not check children.
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
     * Return the child widths.
     */
    virtual Width::ListPtr get_widths(void) const { return m_widths; }

    /**
     * Return the child items.
     */
    virtual EnumItem::ListPtr get_items(void) const { return m_items; }

    /**
     * Change the child widths.
     */
    virtual void set_widths(Width::ListPtr widths) { m_widths = widths; }

    /**
     * Change the child items.
     */
    virtual void set_items(EnumItem::ListPtr items) { m_items = items; }

    /**
     * Return the property base_type.
     */
    virtual const Base_typeEnum &get_base_type(void) const { return m_base_type; }

    /**
     * Return the property sign.
     */
    virtual const bool &get_sign(void) const { return m_sign; }

    /**
     * Change the property base_type.
     */
    virtual void set_base_type(const Base_typeEnum &base_type) { m_base_type = base_type; }

    /**
     * Change the property sign.
     */
    virtual void set_sign(const bool &sign) { m_sign = sign; }

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

    Width::ListPtr m_widths{};
    EnumItem::ListPtr m_items{};
    Base_typeEnum m_base_type{};
    bool m_sign{};
};

std::ostream &operator<<(std::ostream &os, const EnumDef &p);
std::ostream &operator<<(std::ostream &os, const EnumDef::Ptr p);

std::ostream &operator<<(std::ostream &os, const EnumDef::Base_typeEnum p);

} // namespace AST
} // namespace Veriparse

#endif