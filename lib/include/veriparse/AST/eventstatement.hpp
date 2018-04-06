#ifndef VERIPARSE_AST_EVENTSTATEMENT_HPP
#define VERIPARSE_AST_EVENTSTATEMENT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/senslist.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class EventStatement: public Node {
		public:
			using Ptr = typename NodePointers<EventStatement>::Ptr;
			using List = typename NodePointers<EventStatement>::List;
			using ListPtr = typename NodePointers<EventStatement>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::EventStatement.
			 */
			EventStatement(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::EventStatement.
			 */
			EventStatement(const Senslist::Ptr senslist, const Node::Ptr statement, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual EventStatement &operator=(const EventStatement &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the EventStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const EventStatement &rhs) const;

			/**
			 * Return true if the EventStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the EventStatement nodes are the same, do not check children.
			 */
			virtual bool operator!=(const EventStatement &rhs) const;

			/**
			 * Return true if the EventStatement nodes are the same, do not check children.
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
			 * Return the child senslist.
			 */
			virtual Senslist::Ptr get_senslist(void) const {return m_senslist;}
			
			/**
			 * Return the child statement.
			 */
			virtual Node::Ptr get_statement(void) const {return m_statement;}
			
			/**
			 * Change the child senslist.
			 */
			virtual void set_senslist(Senslist::Ptr senslist) {
				m_senslist = senslist;
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

			
			Senslist::Ptr m_senslist;
			Node::Ptr m_statement;
		};

		std::ostream & operator<<(std::ostream &os, const EventStatement &p);
		std::ostream & operator<<(std::ostream &os, const EventStatement::Ptr p);
		


	}
}


#endif