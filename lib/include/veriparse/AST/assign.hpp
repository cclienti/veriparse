#ifndef VERIPARSE_AST_ASSIGN_HPP
#define VERIPARSE_AST_ASSIGN_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/delaystatement.hpp>
#include <veriparse/AST/lvalue.hpp>
#include <veriparse/AST/rvalue.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Assign: public Node {
public:
	using Ptr = typename NodePointers<Assign>::Ptr;
	using List = typename NodePointers<Assign>::List;
	using ListPtr = typename NodePointers<Assign>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Assign.
	 */
	Assign(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Assign.
	 */
	Assign(const Lvalue::Ptr left, const Rvalue::Ptr right, const DelayStatement::Ptr ldelay, const DelayStatement::Ptr rdelay, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Assign &operator=(const Assign &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Assign nodes are the same, do not check children.
	 */
	virtual bool operator==(const Assign &rhs) const;

	/**
	 * Return true if the Assign nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Assign nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Assign &rhs) const;

	/**
	 * Return true if the Assign nodes are the same, do not check children.
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
	 * Return the child left.
	 */
	virtual Lvalue::Ptr get_left(void) const {return m_left;}
	
	/**
	 * Return the child right.
	 */
	virtual Rvalue::Ptr get_right(void) const {return m_right;}
	
	/**
	 * Return the child ldelay.
	 */
	virtual DelayStatement::Ptr get_ldelay(void) const {return m_ldelay;}
	
	/**
	 * Return the child rdelay.
	 */
	virtual DelayStatement::Ptr get_rdelay(void) const {return m_rdelay;}
	
	/**
	 * Change the child left.
	 */
	virtual void set_left(Lvalue::Ptr left) {
		m_left = left;
	}
	
	/**
	 * Change the child right.
	 */
	virtual void set_right(Rvalue::Ptr right) {
		m_right = right;
	}
	
	/**
	 * Change the child ldelay.
	 */
	virtual void set_ldelay(DelayStatement::Ptr ldelay) {
		m_ldelay = ldelay;
	}
	
	/**
	 * Change the child rdelay.
	 */
	virtual void set_rdelay(DelayStatement::Ptr rdelay) {
		m_rdelay = rdelay;
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

	
	Lvalue::Ptr m_left;
	Rvalue::Ptr m_right;
	DelayStatement::Ptr m_ldelay;
	DelayStatement::Ptr m_rdelay;
};

std::ostream & operator<<(std::ostream &os, const Assign &p);
std::ostream & operator<<(std::ostream &os, const Assign::Ptr p);



}
}


#endif