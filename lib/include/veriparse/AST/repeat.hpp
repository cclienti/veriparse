#ifndef VERIPARSE_AST_REPEAT_HPP
#define VERIPARSE_AST_REPEAT_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class Repeat: public Node {
		public:
			using Ptr = typename NodePointers<Repeat>::Ptr;
			using List = typename NodePointers<Repeat>::List;
			using ListPtr = typename NodePointers<Repeat>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Repeat.
			 */
			Repeat(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Repeat.
			 */
			Repeat(const Node::Ptr value, const Node::Ptr times, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Repeat &operator=(const Repeat &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Repeat nodes are the same, do not check children.
			 */
			virtual bool operator==(const Repeat &rhs) const;

			/**
			 * Return true if the Repeat nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Repeat nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Repeat &rhs) const;

			/**
			 * Return true if the Repeat nodes are the same, do not check children.
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
			 * Return the child value.
			 */
			virtual Node::Ptr get_value(void) const {return m_value;}
			
			/**
			 * Return the child times.
			 */
			virtual Node::Ptr get_times(void) const {return m_times;}
			
			/**
			 * Change the child value.
			 */
			virtual void set_value(Node::Ptr value) {
				m_value = value;
			}
			
			/**
			 * Change the child times.
			 */
			virtual void set_times(Node::Ptr times) {
				m_times = times;
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

			
			Node::Ptr m_value;
			Node::Ptr m_times;
		};

		std::ostream & operator<<(std::ostream &os, const Repeat &p);
		std::ostream & operator<<(std::ostream &os, const Repeat::Ptr p);
		


	}
}


#endif