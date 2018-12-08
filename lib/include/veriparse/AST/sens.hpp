#ifndef VERIPARSE_AST_SENS_HPP
#define VERIPARSE_AST_SENS_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Sens: public Node {
public:
	using Ptr = typename NodePointers<Sens>::Ptr;
	using List = typename NodePointers<Sens>::List;
	using ListPtr = typename NodePointers<Sens>::ListPtr;

	enum class TypeEnum {
		ALL, 
		POSEDGE, 
		NEGEDGE, 
		NONE
	};

	/**
	 * Constructor, m_node_type is set to NodeType::Sens.
	 */
	Sens(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Sens.
	 */
	Sens(const Node::Ptr sig, const TypeEnum &type, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Sens &operator=(const Sens &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Sens nodes are the same, do not check children.
	 */
	virtual bool operator==(const Sens &rhs) const;

	/**
	 * Return true if the Sens nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Sens nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Sens &rhs) const;

	/**
	 * Return true if the Sens nodes are the same, do not check children.
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
	 * Return the child sig.
	 */
	virtual Node::Ptr get_sig(void) const {return m_sig;}
	
	/**
	 * Change the child sig.
	 */
	virtual void set_sig(Node::Ptr sig) {
		m_sig = sig;
	}
	
	/**
	 * Return the property type.
	 */
	virtual const TypeEnum &get_type(void) const {return m_type;}
	
	/**
	 * Change the property type.
	 */
	virtual void set_type(const TypeEnum &type) {m_type = type;}
	

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

	
	Node::Ptr m_sig;
	TypeEnum m_type;
};

std::ostream & operator<<(std::ostream &os, const Sens &p);
std::ostream & operator<<(std::ostream &os, const Sens::Ptr p);

std::ostream & operator<<(std::ostream &os, const Sens::TypeEnum p);



}
}


#endif