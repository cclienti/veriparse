#ifndef VERIPARSE_AST_IDENTIFIERSCOPELABEL_HPP
#define VERIPARSE_AST_IDENTIFIERSCOPELABEL_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class IdentifierScopeLabel: public Node {
public:
	using Ptr = typename NodePointers<IdentifierScopeLabel>::Ptr;
	using List = typename NodePointers<IdentifierScopeLabel>::List;
	using ListPtr = typename NodePointers<IdentifierScopeLabel>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::IdentifierScopeLabel.
	 */
	IdentifierScopeLabel(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::IdentifierScopeLabel.
	 */
	IdentifierScopeLabel(const Node::Ptr loop, const std::string &scope, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual IdentifierScopeLabel &operator=(const IdentifierScopeLabel &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the IdentifierScopeLabel nodes are the same, do not check children.
	 */
	virtual bool operator==(const IdentifierScopeLabel &rhs) const;

	/**
	 * Return true if the IdentifierScopeLabel nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the IdentifierScopeLabel nodes are the same, do not check children.
	 */
	virtual bool operator!=(const IdentifierScopeLabel &rhs) const;

	/**
	 * Return true if the IdentifierScopeLabel nodes are the same, do not check children.
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
	 * Return the child loop.
	 */
	virtual Node::Ptr get_loop(void) const {return m_loop;}
	
	/**
	 * Change the child loop.
	 */
	virtual void set_loop(Node::Ptr loop) {
		m_loop = loop;
	}
	
	/**
	 * Return the property scope.
	 */
	virtual const std::string &get_scope(void) const {return m_scope;}
	
	/**
	 * Change the property scope.
	 */
	virtual void set_scope(const std::string &scope) {m_scope = scope;}
	

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

	
	Node::Ptr m_loop;
	std::string m_scope;
};

std::ostream & operator<<(std::ostream &os, const IdentifierScopeLabel &p);
std::ostream & operator<<(std::ostream &os, const IdentifierScopeLabel::Ptr p);



}
}


#endif