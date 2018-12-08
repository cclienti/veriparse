#ifndef VERIPARSE_AST_INTCONSTN_HPP
#define VERIPARSE_AST_INTCONSTN_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/constant.hpp>

#include <gmp.h>
#include <gmpxx.h>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class IntConstN: public Constant {
public:
	using Ptr = typename NodePointers<IntConstN>::Ptr;
	using List = typename NodePointers<IntConstN>::List;
	using ListPtr = typename NodePointers<IntConstN>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::IntConstN.
	 */
	IntConstN(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::IntConstN.
	 */
	IntConstN(const int &base, const int &size, const bool &sign, const mpz_class &value, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual IntConstN &operator=(const IntConstN &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the IntConstN nodes are the same, do not check children.
	 */
	virtual bool operator==(const IntConstN &rhs) const;

	/**
	 * Return true if the IntConstN nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the IntConstN nodes are the same, do not check children.
	 */
	virtual bool operator!=(const IntConstN &rhs) const;

	/**
	 * Return true if the IntConstN nodes are the same, do not check children.
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
	 * Return the property base.
	 */
	virtual const int &get_base(void) const {return m_base;}
	
	/**
	 * Return the property size.
	 */
	virtual const int &get_size(void) const {return m_size;}
	
	/**
	 * Return the property sign.
	 */
	virtual const bool &get_sign(void) const {return m_sign;}
	
	/**
	 * Return the property value.
	 */
	virtual const mpz_class &get_value(void) const {return m_value;}
	
	/**
	 * Change the property base.
	 */
	virtual void set_base(const int &base) {m_base = base;}
	
	/**
	 * Change the property size.
	 */
	virtual void set_size(const int &size) {m_size = size;}
	
	/**
	 * Change the property sign.
	 */
	virtual void set_sign(const bool &sign) {m_sign = sign;}
	
	/**
	 * Change the property value.
	 */
	virtual void set_value(const mpz_class &value) {m_value = value;}
	

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

	
	int m_base;
	int m_size;
	bool m_sign;
	mpz_class m_value;
};

std::ostream & operator<<(std::ostream &os, const IntConstN &p);
std::ostream & operator<<(std::ostream &os, const IntConstN::Ptr p);



}
}


#endif