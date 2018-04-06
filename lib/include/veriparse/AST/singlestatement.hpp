#ifndef VERIPARSE_AST_SINGLESTATEMENT_HPP
#define VERIPARSE_AST_SINGLESTATEMENT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/delaystatement.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class SingleStatement: public Node {
		public:
			using Ptr = typename NodePointers<SingleStatement>::Ptr;
			using List = typename NodePointers<SingleStatement>::List;
			using ListPtr = typename NodePointers<SingleStatement>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::SingleStatement.
			 */
			SingleStatement(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::SingleStatement.
			 */
			SingleStatement(const Node::Ptr statement, const DelayStatement::Ptr delay, const std::string &scope, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual SingleStatement &operator=(const SingleStatement &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the SingleStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const SingleStatement &rhs) const;

			/**
			 * Return true if the SingleStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the SingleStatement nodes are the same, do not check children.
			 */
			virtual bool operator!=(const SingleStatement &rhs) const;

			/**
			 * Return true if the SingleStatement nodes are the same, do not check children.
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
			 * Return the child statement.
			 */
			virtual Node::Ptr get_statement(void) const {return m_statement;}
			
			/**
			 * Return the child delay.
			 */
			virtual DelayStatement::Ptr get_delay(void) const {return m_delay;}
			
			/**
			 * Change the child statement.
			 */
			virtual void set_statement(Node::Ptr statement) {
				m_statement = statement;
			}
			
			/**
			 * Change the child delay.
			 */
			virtual void set_delay(DelayStatement::Ptr delay) {
				m_delay = delay;
			}
			
			/**
			 * Return the property scope.
			 */
			virtual const std::string &get_scope(void) const {return m_scope;}
			
			/**
			 * Change the property scope.
			 */
			virtual void set_scope(const std::string &scope) {m_scope = scope;}
			

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

			
			Node::Ptr m_statement;
			DelayStatement::Ptr m_delay;
			std::string m_scope;
		};

		std::ostream & operator<<(std::ostream &os, const SingleStatement &p);
		std::ostream & operator<<(std::ostream &os, const SingleStatement::Ptr p);
		


	}
}


#endif