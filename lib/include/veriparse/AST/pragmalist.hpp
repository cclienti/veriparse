#ifndef VERIPARSE_AST_PRAGMALIST_HPP
#define VERIPARSE_AST_PRAGMALIST_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/pragma.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Pragmalist: public Node {
public:
	using Ptr = typename NodePointers<Pragmalist>::Ptr;
	using List = typename NodePointers<Pragmalist>::List;
	using ListPtr = typename NodePointers<Pragmalist>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Pragmalist.
	 */
	Pragmalist(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Pragmalist.
	 */
	Pragmalist(const Pragma::ListPtr pragmas, const Node::ListPtr statements, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Pragmalist &operator=(const Pragmalist &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Pragmalist nodes are the same, do not check children.
	 */
	virtual bool operator==(const Pragmalist &rhs) const;

	/**
	 * Return true if the Pragmalist nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Pragmalist nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Pragmalist &rhs) const;

	/**
	 * Return true if the Pragmalist nodes are the same, do not check children.
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
	 * Return the child pragmas.
	 */
	virtual Pragma::ListPtr get_pragmas(void) const {return m_pragmas;}
	
	/**
	 * Return the child statements.
	 */
	virtual Node::ListPtr get_statements(void) const {return m_statements;}
	
	/**
	 * Change the child pragmas.
	 */
	virtual void set_pragmas(Pragma::ListPtr pragmas) {
		m_pragmas = pragmas;
	}
	
	/**
	 * Change the child statements.
	 */
	virtual void set_statements(Node::ListPtr statements) {
		m_statements = statements;
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

	
	Pragma::ListPtr m_pragmas;
	Node::ListPtr m_statements;
};

std::ostream & operator<<(std::ostream &os, const Pragmalist &p);
std::ostream & operator<<(std::ostream &os, const Pragmalist::Ptr p);



}
}


#endif