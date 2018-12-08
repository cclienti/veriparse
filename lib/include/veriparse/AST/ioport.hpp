#ifndef VERIPARSE_AST_IOPORT_HPP
#define VERIPARSE_AST_IOPORT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/iodir.hpp>
#include <veriparse/AST/variable.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Ioport: public Node {
public:
	using Ptr = typename NodePointers<Ioport>::Ptr;
	using List = typename NodePointers<Ioport>::List;
	using ListPtr = typename NodePointers<Ioport>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Ioport.
	 */
	Ioport(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Ioport.
	 */
	Ioport(const IODir::Ptr first, const Variable::Ptr second, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Ioport &operator=(const Ioport &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Ioport nodes are the same, do not check children.
	 */
	virtual bool operator==(const Ioport &rhs) const;

	/**
	 * Return true if the Ioport nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Ioport nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Ioport &rhs) const;

	/**
	 * Return true if the Ioport nodes are the same, do not check children.
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
	 * Return the child first.
	 */
	virtual IODir::Ptr get_first(void) const {return m_first;}
	
	/**
	 * Return the child second.
	 */
	virtual Variable::Ptr get_second(void) const {return m_second;}
	
	/**
	 * Change the child first.
	 */
	virtual void set_first(IODir::Ptr first) {
		m_first = first;
	}
	
	/**
	 * Change the child second.
	 */
	virtual void set_second(Variable::Ptr second) {
		m_second = second;
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

	
	IODir::Ptr m_first;
	Variable::Ptr m_second;
};

std::ostream & operator<<(std::ostream &os, const Ioport &p);
std::ostream & operator<<(std::ostream &os, const Ioport::Ptr p);



}
}


#endif