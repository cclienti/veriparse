#ifndef VERIPARSE_AST_SENSLIST_HPP
#define VERIPARSE_AST_SENSLIST_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/sens.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class Senslist: public Node {
		public:
			using Ptr = typename NodePointers<Senslist>::Ptr;
			using List = typename NodePointers<Senslist>::List;
			using ListPtr = typename NodePointers<Senslist>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Senslist.
			 */
			Senslist(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Senslist.
			 */
			Senslist(const Sens::ListPtr list, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Senslist &operator=(const Senslist &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Senslist nodes are the same, do not check children.
			 */
			virtual bool operator==(const Senslist &rhs) const;

			/**
			 * Return true if the Senslist nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Senslist nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Senslist &rhs) const;

			/**
			 * Return true if the Senslist nodes are the same, do not check children.
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
			 * Return the child list.
			 */
			virtual Sens::ListPtr get_list(void) const {return m_list;}
			
			/**
			 * Change the child list.
			 */
			virtual void set_list(Sens::ListPtr list) {
				m_list = list;
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

			
			Sens::ListPtr m_list;
		};

		std::ostream & operator<<(std::ostream &os, const Senslist &p);
		std::ostream & operator<<(std::ostream &os, const Senslist::Ptr p);
		


	}
}


#endif