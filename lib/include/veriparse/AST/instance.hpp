#ifndef VERIPARSE_AST_INSTANCE_HPP
#define VERIPARSE_AST_INSTANCE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/length.hpp>
#include <veriparse/AST/paramarg.hpp>
#include <veriparse/AST/portarg.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class Instance: public Node {
		public:
			using Ptr = typename NodePointers<Instance>::Ptr;
			using List = typename NodePointers<Instance>::List;
			using ListPtr = typename NodePointers<Instance>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Instance.
			 */
			Instance(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Instance.
			 */
			Instance(const Length::Ptr array, const ParamArg::ListPtr parameterlist, const PortArg::ListPtr portlist, const std::string &module, const std::string &name, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Instance &operator=(const Instance &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Instance nodes are the same, do not check children.
			 */
			virtual bool operator==(const Instance &rhs) const;

			/**
			 * Return true if the Instance nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Instance nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Instance &rhs) const;

			/**
			 * Return true if the Instance nodes are the same, do not check children.
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
			 * Return the child array.
			 */
			virtual Length::Ptr get_array(void) const {return m_array;}
			
			/**
			 * Return the child parameterlist.
			 */
			virtual ParamArg::ListPtr get_parameterlist(void) const {return m_parameterlist;}
			
			/**
			 * Return the child portlist.
			 */
			virtual PortArg::ListPtr get_portlist(void) const {return m_portlist;}
			
			/**
			 * Change the child array.
			 */
			virtual void set_array(Length::Ptr array) {
				m_array = array;
			}
			
			/**
			 * Change the child parameterlist.
			 */
			virtual void set_parameterlist(ParamArg::ListPtr parameterlist) {
				m_parameterlist = parameterlist;
			}
			
			/**
			 * Change the child portlist.
			 */
			virtual void set_portlist(PortArg::ListPtr portlist) {
				m_portlist = portlist;
			}
			
			/**
			 * Return the property module.
			 */
			virtual const std::string &get_module(void) const {return m_module;}
			
			/**
			 * Return the property name.
			 */
			virtual const std::string &get_name(void) const {return m_name;}
			
			/**
			 * Change the property module.
			 */
			virtual void set_module(const std::string &module) {m_module = module;}
			
			/**
			 * Change the property name.
			 */
			virtual void set_name(const std::string &name) {m_name = name;}
			

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

			
			Length::Ptr m_array;
			ParamArg::ListPtr m_parameterlist;
			PortArg::ListPtr m_portlist;
			std::string m_module;
			std::string m_name;
		};

		std::ostream & operator<<(std::ostream &os, const Instance &p);
		std::ostream & operator<<(std::ostream &os, const Instance::Ptr p);
		


	}
}


#endif