#ifndef VERIPARSE_AST_TASKCALL_HPP
#define VERIPARSE_AST_TASKCALL_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class TaskCall: public Node {
		public:
			using Ptr = typename NodePointers<TaskCall>::Ptr;
			using List = typename NodePointers<TaskCall>::List;
			using ListPtr = typename NodePointers<TaskCall>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::TaskCall.
			 */
			TaskCall(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::TaskCall.
			 */
			TaskCall(const Node::ListPtr args, const std::string &name, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual TaskCall &operator=(const TaskCall &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the TaskCall nodes are the same, do not check children.
			 */
			virtual bool operator==(const TaskCall &rhs) const;

			/**
			 * Return true if the TaskCall nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the TaskCall nodes are the same, do not check children.
			 */
			virtual bool operator!=(const TaskCall &rhs) const;

			/**
			 * Return true if the TaskCall nodes are the same, do not check children.
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
			 * Return the child args.
			 */
			virtual Node::ListPtr get_args(void) const {return m_args;}
			
			/**
			 * Change the child args.
			 */
			virtual void set_args(Node::ListPtr args) {
				m_args = args;
			}
			
			/**
			 * Return the property name.
			 */
			virtual const std::string &get_name(void) const {return m_name;}
			
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

			
			Node::ListPtr m_args;
			std::string m_name;
		};

		std::ostream & operator<<(std::ostream &os, const TaskCall &p);
		std::ostream & operator<<(std::ostream &os, const TaskCall::Ptr p);
		


	}
}


#endif