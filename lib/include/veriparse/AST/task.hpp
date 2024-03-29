#ifndef VERIPARSE_AST_TASK_HPP
#define VERIPARSE_AST_TASK_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Task: public Node {
public:
	using Ptr = typename NodePointers<Task>::Ptr;
	using List = typename NodePointers<Task>::List;
	using ListPtr = typename NodePointers<Task>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Task.
	 */
	Task(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Task.
	 */
	Task(const Node::ListPtr ports, const Node::ListPtr statements, const std::string &name, const bool &automatic, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Task &operator=(const Task &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Task nodes are the same, do not check children.
	 */
	virtual bool operator==(const Task &rhs) const;

	/**
	 * Return true if the Task nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Task nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Task &rhs) const;

	/**
	 * Return true if the Task nodes are the same, do not check children.
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
	 * Return the child ports.
	 */
	virtual Node::ListPtr get_ports(void) const {return m_ports;}
	
	/**
	 * Return the child statements.
	 */
	virtual Node::ListPtr get_statements(void) const {return m_statements;}
	
	/**
	 * Change the child ports.
	 */
	virtual void set_ports(Node::ListPtr ports) {
		m_ports = ports;
	}
	
	/**
	 * Change the child statements.
	 */
	virtual void set_statements(Node::ListPtr statements) {
		m_statements = statements;
	}
	
	/**
	 * Return the property name.
	 */
	virtual const std::string &get_name(void) const {return m_name;}
	
	/**
	 * Return the property automatic.
	 */
	virtual const bool &get_automatic(void) const {return m_automatic;}
	
	/**
	 * Change the property name.
	 */
	virtual void set_name(const std::string &name) {m_name = name;}
	
	/**
	 * Change the property automatic.
	 */
	virtual void set_automatic(const bool &automatic) {m_automatic = automatic;}
	

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

	
	Node::ListPtr m_ports;
	Node::ListPtr m_statements;
	std::string m_name;
	bool m_automatic;
};

std::ostream & operator<<(std::ostream &os, const Task &p);
std::ostream & operator<<(std::ostream &os, const Task::Ptr p);



}
}


#endif