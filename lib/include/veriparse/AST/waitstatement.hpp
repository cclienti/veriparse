#ifndef VERIPARSE_AST_WAITSTATEMENT_HPP
#define VERIPARSE_AST_WAITSTATEMENT_HPP

#include <veriparse/AST/node.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class WaitStatement: public Node {
		public:
			using Ptr = typename NodePointers<WaitStatement>::Ptr;
			using List = typename NodePointers<WaitStatement>::List;
			using ListPtr = typename NodePointers<WaitStatement>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::WaitStatement.
			 */
			WaitStatement(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::WaitStatement.
			 */
			WaitStatement(const Node::Ptr cond, const Node::Ptr statement, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual WaitStatement &operator=(const WaitStatement &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the WaitStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const WaitStatement &rhs) const;

			/**
			 * Return true if the WaitStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the WaitStatement nodes are the same, do not check children.
			 */
			virtual bool operator!=(const WaitStatement &rhs) const;

			/**
			 * Return true if the WaitStatement nodes are the same, do not check children.
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
			 * Return the child cond.
			 */
			virtual Node::Ptr get_cond(void) const {return m_cond;}
			
			/**
			 * Return the child statement.
			 */
			virtual Node::Ptr get_statement(void) const {return m_statement;}
			
			/**
			 * Change the child cond.
			 */
			virtual void set_cond(Node::Ptr cond) {
				m_cond = cond;
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

			
			Node::Ptr m_cond;
			Node::Ptr m_statement;
		};

		std::ostream & operator<<(std::ostream &os, const WaitStatement &p);
		std::ostream & operator<<(std::ostream &os, const WaitStatement::Ptr p);
		


	}
}


#endif