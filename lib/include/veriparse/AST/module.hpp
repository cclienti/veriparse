#ifndef VERIPARSE_AST_MODULE_HPP
#define VERIPARSE_AST_MODULE_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/parameter.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class Module: public Node {
		public:
			using Ptr = typename NodePointers<Module>::Ptr;
			using List = typename NodePointers<Module>::List;
			using ListPtr = typename NodePointers<Module>::ListPtr;

			enum class Default_nettypeEnum {
				INTEGER, 
				REAL, 
				REG, 
				TRI, 
				WIRE, 
				SUPPLY0, 
				SUPPLY1, 
				NONE
			};

			/**
			 * Constructor, m_node_type is set to NodeType::Module.
			 */
			Module(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Module.
			 */
			Module(const Parameter::ListPtr params, const Node::ListPtr ports, const Node::ListPtr items, const std::string &name, const Default_nettypeEnum &default_nettype, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Module &operator=(const Module &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Module nodes are the same, do not check children.
			 */
			virtual bool operator==(const Module &rhs) const;

			/**
			 * Return true if the Module nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Module nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Module &rhs) const;

			/**
			 * Return true if the Module nodes are the same, do not check children.
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
			 * Return the child params.
			 */
			virtual Parameter::ListPtr get_params(void) const {return m_params;}
			
			/**
			 * Return the child ports.
			 */
			virtual Node::ListPtr get_ports(void) const {return m_ports;}
			
			/**
			 * Return the child items.
			 */
			virtual Node::ListPtr get_items(void) const {return m_items;}
			
			/**
			 * Change the child params.
			 */
			virtual void set_params(Parameter::ListPtr params) {
				m_params = params;
			}
			
			/**
			 * Change the child ports.
			 */
			virtual void set_ports(Node::ListPtr ports) {
				m_ports = ports;
			}
			
			/**
			 * Change the child items.
			 */
			virtual void set_items(Node::ListPtr items) {
				m_items = items;
			}
			
			/**
			 * Return the property name.
			 */
			virtual const std::string &get_name(void) const {return m_name;}
			
			/**
			 * Return the property default_nettype.
			 */
			virtual const Default_nettypeEnum &get_default_nettype(void) const {return m_default_nettype;}
			
			/**
			 * Change the property name.
			 */
			virtual void set_name(const std::string &name) {m_name = name;}
			
			/**
			 * Change the property default_nettype.
			 */
			virtual void set_default_nettype(const Default_nettypeEnum &default_nettype) {m_default_nettype = default_nettype;}
			

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

			
			Parameter::ListPtr m_params;
			Node::ListPtr m_ports;
			Node::ListPtr m_items;
			std::string m_name;
			Default_nettypeEnum m_default_nettype;
		};

		std::ostream & operator<<(std::ostream &os, const Module &p);
		std::ostream & operator<<(std::ostream &os, const Module::Ptr p);
		
		std::ostream & operator<<(std::ostream &os, const Module::Default_nettypeEnum p);
		


	}
}


#endif