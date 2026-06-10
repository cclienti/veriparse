// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_AST_NETTYPEDECL_HPP
#define VERIPARSE_AST_NETTYPEDECL_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/declaration.hpp>

#include <veriparse/AST/datatype.hpp>
#include <veriparse/AST/identifier.hpp>

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

class NetTypeDecl : public Declaration
{
public:
    using Ptr = typename NodePointers<NetTypeDecl>::Ptr;
    using List = typename NodePointers<NetTypeDecl>::List;
    using ListPtr = typename NodePointers<NetTypeDecl>::ListPtr;
    using Declaration::operator=;
    using Declaration::operator==;
    using Declaration::operator!=;

    /**
     * Constructor, m_node_type is set to NodeType::NetTypeDecl.
     */
    NetTypeDecl(const std::string &filename = "", uint32_t line = 0);

    /**
     * Constructor, m_node_type is set to NodeType::NetTypeDecl.
     */
    NetTypeDecl(const Identifier::Ptr resolver, const DataType::Ptr type, const std::string &name,
                const std::string &filename = "", uint32_t line = 0);

    /**
     * Assignment operator, do not affect children.
     */
    virtual NetTypeDecl &operator=(const NetTypeDecl &rhs);

    /**
     * Assignment operator, do not affect children.
     */
    virtual Node &operator=(const Node &rhs) override;

    /**
     * Return true if the NetTypeDecl nodes are the same, do not check children.
     */
    virtual bool operator==(const NetTypeDecl &rhs) const;

    /**
     * Return true if the NetTypeDecl nodes are the same, do not check children.
     */
    virtual bool operator==(const Node &rhs) const override;

    /**
     * Return true if the NetTypeDecl nodes are the same, do not check children.
     */
    virtual bool operator!=(const NetTypeDecl &rhs) const;

    /**
     * Return true if the NetTypeDecl nodes are the same, do not check children.
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
     * Return the child resolver.
     */
    virtual Identifier::Ptr get_resolver(void) const { return m_resolver; }

    /**
     * Change the child resolver.
     */
    virtual void set_resolver(Identifier::Ptr resolver) { m_resolver = resolver; }

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

    Identifier::Ptr m_resolver{};
};

std::ostream &operator<<(std::ostream &os, const NetTypeDecl &p);
std::ostream &operator<<(std::ostream &os, const NetTypeDecl::Ptr p);

} // namespace AST
} // namespace Veriparse

#endif