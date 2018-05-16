#ifndef VERIPARSE_AST_REPEATSTATEMENT_HPP
#define VERIPARSE_AST_REPEATSTATEMENT_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class RepeatStatement: public Node {
		public:
			using Ptr = typename NodePointers<RepeatStatement>::Ptr;
			using List = typename NodePointers<RepeatStatement>::List;
			using ListPtr = typename NodePointers<RepeatStatement>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::RepeatStatement.
			 */
			RepeatStatement(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::RepeatStatement.
			 */
			RepeatStatement(const Node::Ptr times, const Node::Ptr statement, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual RepeatStatement &operator=(const RepeatStatement &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the RepeatStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const RepeatStatement &rhs) const;

			/**
			 * Return true if the RepeatStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the RepeatStatement nodes are the same, do not check children.
			 */
			virtual bool operator!=(const RepeatStatement &rhs) const;

			/**
			 * Return true if the RepeatStatement nodes are the same, do not check children.
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
			 * Return the child times.
			 */
			virtual Node::Ptr get_times(void) const {return m_times;}
			
			/**
			 * Return the child statement.
			 */
			virtual Node::Ptr get_statement(void) const {return m_statement;}
			
			/**
			 * Change the child times.
			 */
			virtual void set_times(Node::Ptr times) {
				m_times = times;
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

			
			Node::Ptr m_times;
			Node::Ptr m_statement;
		};

		std::ostream & operator<<(std::ostream &os, const RepeatStatement &p);
		std::ostream & operator<<(std::ostream &os, const RepeatStatement::Ptr p);
		


	}
}


#endif