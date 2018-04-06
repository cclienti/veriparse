#ifndef VERIPARSE_AST_LESSTHAN_HPP
#define VERIPARSE_AST_LESSTHAN_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/operator.hpp>


#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class LessThan: public Operator {
		public:
			using Ptr = typename NodePointers<LessThan>::Ptr;
			using List = typename NodePointers<LessThan>::List;
			using ListPtr = typename NodePointers<LessThan>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::LessThan.
			 */
			LessThan(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::LessThan.
			 */
			LessThan(const Node::Ptr left, const Node::Ptr right, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual LessThan &operator=(const LessThan &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the LessThan nodes are the same, do not check children.
			 */
			virtual bool operator==(const LessThan &rhs) const;

			/**
			 * Return true if the LessThan nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the LessThan nodes are the same, do not check children.
			 */
			virtual bool operator!=(const LessThan &rhs) const;

			/**
			 * Return true if the LessThan nodes are the same, do not check children.
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

			
		};

		std::ostream & operator<<(std::ostream &os, const LessThan &p);
		std::ostream & operator<<(std::ostream &os, const LessThan::Ptr p);
		


	}
}


#endif