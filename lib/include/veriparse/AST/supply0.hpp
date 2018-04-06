#ifndef VERIPARSE_AST_SUPPLY0_HPP
#define VERIPARSE_AST_SUPPLY0_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/net.hpp>

#include <veriparse/AST/delaystatement.hpp>
#include <veriparse/AST/length.hpp>
#include <veriparse/AST/rvalue.hpp>
#include <veriparse/AST/width.hpp>

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class Supply0: public Net {
		public:
			using Ptr = typename NodePointers<Supply0>::Ptr;
			using List = typename NodePointers<Supply0>::List;
			using ListPtr = typename NodePointers<Supply0>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Supply0.
			 */
			Supply0(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Supply0.
			 */
			Supply0(const Width::ListPtr widths, const DelayStatement::Ptr ldelay, const DelayStatement::Ptr rdelay, const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign, const std::string &name, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Supply0 &operator=(const Supply0 &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Supply0 nodes are the same, do not check children.
			 */
			virtual bool operator==(const Supply0 &rhs) const;

			/**
			 * Return true if the Supply0 nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Supply0 nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Supply0 &rhs) const;

			/**
			 * Return true if the Supply0 nodes are the same, do not check children.
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

		std::ostream & operator<<(std::ostream &os, const Supply0 &p);
		std::ostream & operator<<(std::ostream &os, const Supply0::Ptr p);
		


	}
}


#endif