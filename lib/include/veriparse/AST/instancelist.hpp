#ifndef VERIPARSE_AST_INSTANCELIST_HPP
#define VERIPARSE_AST_INSTANCELIST_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/instance.hpp>
#include <veriparse/AST/paramarg.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class Instancelist: public Node {
public:
	using Ptr = typename NodePointers<Instancelist>::Ptr;
	using List = typename NodePointers<Instancelist>::List;
	using ListPtr = typename NodePointers<Instancelist>::ListPtr;

	/**
	 * Constructor, m_node_type is set to NodeType::Instancelist.
	 */
	Instancelist(const std::string &filename="", uint32_t line=0);
	
	/**
	 * Constructor, m_node_type is set to NodeType::Instancelist.
	 */
	Instancelist(const ParamArg::ListPtr parameterlist, const Instance::ListPtr instances, const std::string &module, const std::string &filename="", uint32_t line=0);
	

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Instancelist &operator=(const Instancelist &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the Instancelist nodes are the same, do not check children.
	 */
	virtual bool operator==(const Instancelist &rhs) const;

	/**
	 * Return true if the Instancelist nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the Instancelist nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Instancelist &rhs) const;

	/**
	 * Return true if the Instancelist nodes are the same, do not check children.
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
	 * Return the child parameterlist.
	 */
	virtual ParamArg::ListPtr get_parameterlist(void) const {return m_parameterlist;}
	
	/**
	 * Return the child instances.
	 */
	virtual Instance::ListPtr get_instances(void) const {return m_instances;}
	
	/**
	 * Change the child parameterlist.
	 */
	virtual void set_parameterlist(ParamArg::ListPtr parameterlist) {
		m_parameterlist = parameterlist;
	}
	
	/**
	 * Change the child instances.
	 */
	virtual void set_instances(Instance::ListPtr instances) {
		m_instances = instances;
	}
	
	/**
	 * Return the property module.
	 */
	virtual const std::string &get_module(void) const {return m_module;}
	
	/**
	 * Change the property module.
	 */
	virtual void set_module(const std::string &module) {m_module = module;}
	

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

	
	ParamArg::ListPtr m_parameterlist;
	Instance::ListPtr m_instances;
	std::string m_module;
};

std::ostream & operator<<(std::ostream &os, const Instancelist &p);
std::ostream & operator<<(std::ostream &os, const Instancelist::Ptr p);



}
}


#endif