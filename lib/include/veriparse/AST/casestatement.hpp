#ifndef VERIPARSE_AST_CASESTATEMENT_HPP
#define VERIPARSE_AST_CASESTATEMENT_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/case.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class CaseStatement: public Node {
		public:
			using Ptr = typename NodePointers<CaseStatement>::Ptr;
			using List = typename NodePointers<CaseStatement>::List;
			using ListPtr = typename NodePointers<CaseStatement>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::CaseStatement.
			 */
			CaseStatement(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::CaseStatement.
			 */
			CaseStatement(const Node::Ptr comp, const Case::ListPtr caselist, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual CaseStatement &operator=(const CaseStatement &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the CaseStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const CaseStatement &rhs) const;

			/**
			 * Return true if the CaseStatement nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the CaseStatement nodes are the same, do not check children.
			 */
			virtual bool operator!=(const CaseStatement &rhs) const;

			/**
			 * Return true if the CaseStatement nodes are the same, do not check children.
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
			 * Return the child comp.
			 */
			virtual Node::Ptr get_comp(void) const {return m_comp;}
			
			/**
			 * Return the child caselist.
			 */
			virtual Case::ListPtr get_caselist(void) const {return m_caselist;}
			
			/**
			 * Change the child comp.
			 */
			virtual void set_comp(Node::Ptr comp) {
				m_comp = comp;
			}
			
			/**
			 * Change the child caselist.
			 */
			virtual void set_caselist(Case::ListPtr caselist) {
				m_caselist = caselist;
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

			
			Node::Ptr m_comp;
			Case::ListPtr m_caselist;
		};

		std::ostream & operator<<(std::ostream &os, const CaseStatement &p);
		std::ostream & operator<<(std::ostream &os, const CaseStatement::Ptr p);
		


	}
}


#endif