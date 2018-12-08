#ifndef VERIPARSE_AST_WIDTH_HPP
#define VERIPARSE_AST_WIDTH_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Width: public Node {
public:
	using Ptr = typename NodePointers<Width>::Ptr;
	using List = typename NodePointers<Width>::List;
	using ListPtr = typename NodePointers<Width>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Width.
	 */
	Width(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Width.
	 */
	Width(const Node::Ptr msb, const Node::Ptr lsb, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Width &operator=(const Width &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Width nodes are the same, do not check children.
	 */
	virtual bool operator==(const Width &rhs) const;

	/**
	 * Return true if the Width nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Width nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Width &rhs) const;

	/**
	 * Return true if the Width nodes are the same, do not check children.
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
	 * Return the child msb.
	 */
	virtual Node::Ptr get_msb(void) const {return m_msb;}
	
	/**
	 * Return the child lsb.
	 */
	virtual Node::Ptr get_lsb(void) const {return m_lsb;}
	
	/**
	 * Change the child msb.
	 */
	virtual void set_msb(Node::Ptr msb) {
		m_msb = msb;
	}
	
	/**
	 * Change the child lsb.
	 */
	virtual void set_lsb(Node::Ptr lsb) {
		m_lsb = lsb;
	}
	

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

	
	Node::Ptr m_msb;
	Node::Ptr m_lsb;
};

std::ostream & operator<<(std::ostream &os, const Width &p);
std::ostream & operator<<(std::ostream &os, const Width::Ptr p);



}
}


#endif