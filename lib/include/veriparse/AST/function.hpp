#ifndef VERIPARSE_AST_FUNCTION_HPP
#define VERIPARSE_AST_FUNCTION_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/width.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Function: public Node {
public:
	using Ptr = typename NodePointers<Function>::Ptr;
	using List = typename NodePointers<Function>::List;
	using ListPtr = typename NodePointers<Function>::ListPtr;

	enum class RettypeEnum {
		INTEGER, 
		REAL, 
		NONE
	};

	/**
	 * Constructor, m_node_type is set to NodeType::Function.
	 */
	Function(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Function.
	 */
	Function(const Width::ListPtr retwidths, const Node::ListPtr ports, const Node::ListPtr statements, const std::string &name, const bool &automatic, const RettypeEnum &rettype, const bool &retsign, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Function &operator=(const Function &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Function nodes are the same, do not check children.
	 */
	virtual bool operator==(const Function &rhs) const;

	/**
	 * Return true if the Function nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Function nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Function &rhs) const;

	/**
	 * Return true if the Function nodes are the same, do not check children.
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
	 * Return the child retwidths.
	 */
	virtual Width::ListPtr get_retwidths(void) const {return m_retwidths;}
	
	/**
	 * Return the child ports.
	 */
	virtual Node::ListPtr get_ports(void) const {return m_ports;}
	
	/**
	 * Return the child statements.
	 */
	virtual Node::ListPtr get_statements(void) const {return m_statements;}
	
	/**
	 * Change the child retwidths.
	 */
	virtual void set_retwidths(Width::ListPtr retwidths) {
		m_retwidths = retwidths;
	}
	
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
	 * Return the property rettype.
	 */
	virtual const RettypeEnum &get_rettype(void) const {return m_rettype;}
	
	/**
	 * Return the property retsign.
	 */
	virtual const bool &get_retsign(void) const {return m_retsign;}
	
	/**
	 * Change the property name.
	 */
	virtual void set_name(const std::string &name) {m_name = name;}
	
	/**
	 * Change the property automatic.
	 */
	virtual void set_automatic(const bool &automatic) {m_automatic = automatic;}
	
	/**
	 * Change the property rettype.
	 */
	virtual void set_rettype(const RettypeEnum &rettype) {m_rettype = rettype;}
	
	/**
	 * Change the property retsign.
	 */
	virtual void set_retsign(const bool &retsign) {m_retsign = retsign;}
	

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

	
	Width::ListPtr m_retwidths;
	Node::ListPtr m_ports;
	Node::ListPtr m_statements;
	std::string m_name;
	bool m_automatic;
	RettypeEnum m_rettype;
	bool m_retsign;
};

std::ostream & operator<<(std::ostream &os, const Function &p);
std::ostream & operator<<(std::ostream &os, const Function::Ptr p);

std::ostream & operator<<(std::ostream &os, const Function::RettypeEnum p);



}
}


#endif