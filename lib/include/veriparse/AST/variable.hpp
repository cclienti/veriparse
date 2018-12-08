#ifndef VERIPARSE_AST_VARIABLE_HPP
#define VERIPARSE_AST_VARIABLE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/variablebase.hpp>

#include <veriparse/AST/length.hpp>
#include <veriparse/AST/rvalue.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Variable: public VariableBase {
public:
	using Ptr = typename NodePointers<Variable>::Ptr;
	using List = typename NodePointers<Variable>::List;
	using ListPtr = typename NodePointers<Variable>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Variable.
	 */
	Variable(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Variable.
	 */
	Variable(const Length::ListPtr lengths, const Rvalue::Ptr right, const std::string &name, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Variable &operator=(const Variable &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Variable nodes are the same, do not check children.
	 */
	virtual bool operator==(const Variable &rhs) const;

	/**
	 * Return true if the Variable nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Variable nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Variable &rhs) const;

	/**
	 * Return true if the Variable nodes are the same, do not check children.
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
	 * Return the child lengths.
	 */
	virtual Length::ListPtr get_lengths(void) const {return m_lengths;}
	
	/**
	 * Return the child right.
	 */
	virtual Rvalue::Ptr get_right(void) const {return m_right;}
	
	/**
	 * Change the child lengths.
	 */
	virtual void set_lengths(Length::ListPtr lengths) {
		m_lengths = lengths;
	}
	
	/**
	 * Change the child right.
	 */
	virtual void set_right(Rvalue::Ptr right) {
		m_right = right;
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

	
	Length::ListPtr m_lengths;
	Rvalue::Ptr m_right;
};

std::ostream & operator<<(std::ostream &os, const Variable &p);
std::ostream & operator<<(std::ostream &os, const Variable::Ptr p);



}
}


#endif