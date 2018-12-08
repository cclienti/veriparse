#ifndef VERIPARSE_AST_FORSTATEMENT_HPP
#define VERIPARSE_AST_FORSTATEMENT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/blockingsubstitution.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class ForStatement: public Node {
public:
	using Ptr = typename NodePointers<ForStatement>::Ptr;
	using List = typename NodePointers<ForStatement>::List;
	using ListPtr = typename NodePointers<ForStatement>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::ForStatement.
	 */
	ForStatement(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::ForStatement.
	 */
	ForStatement(const BlockingSubstitution::Ptr pre, const Node::Ptr cond, const BlockingSubstitution::Ptr post, const Node::Ptr statement, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual ForStatement &operator=(const ForStatement &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the ForStatement nodes are the same, do not check children.
	 */
	virtual bool operator==(const ForStatement &rhs) const;

	/**
	 * Return true if the ForStatement nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the ForStatement nodes are the same, do not check children.
	 */
	virtual bool operator!=(const ForStatement &rhs) const;

	/**
	 * Return true if the ForStatement nodes are the same, do not check children.
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
	 * Return the child pre.
	 */
	virtual BlockingSubstitution::Ptr get_pre(void) const {return m_pre;}
	
	/**
	 * Return the child cond.
	 */
	virtual Node::Ptr get_cond(void) const {return m_cond;}
	
	/**
	 * Return the child post.
	 */
	virtual BlockingSubstitution::Ptr get_post(void) const {return m_post;}
	
	/**
	 * Return the child statement.
	 */
	virtual Node::Ptr get_statement(void) const {return m_statement;}
	
	/**
	 * Change the child pre.
	 */
	virtual void set_pre(BlockingSubstitution::Ptr pre) {
		m_pre = pre;
	}
	
	/**
	 * Change the child cond.
	 */
	virtual void set_cond(Node::Ptr cond) {
		m_cond = cond;
	}
	
	/**
	 * Change the child post.
	 */
	virtual void set_post(BlockingSubstitution::Ptr post) {
		m_post = post;
	}
	
	/**
	 * Change the child statement.
	 */
	virtual void set_statement(Node::Ptr statement) {
		m_statement = statement;
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

	
	BlockingSubstitution::Ptr m_pre;
	Node::Ptr m_cond;
	BlockingSubstitution::Ptr m_post;
	Node::Ptr m_statement;
};

std::ostream & operator<<(std::ostream &os, const ForStatement &p);
std::ostream & operator<<(std::ostream &os, const ForStatement::Ptr p);



}
}


#endif