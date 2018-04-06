#ifndef VERIPARSE_AST_STRINGCONST_HPP
#define VERIPARSE_AST_STRINGCONST_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/constant.hpp>


#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
	namespace AST {

		class StringConst: public Constant {
		public:
			using Ptr = typename NodePointers<StringConst>::Ptr;
			using List = typename NodePointers<StringConst>::List;
			using ListPtr = typename NodePointers<StringConst>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::StringConst.
			 */
			StringConst(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::StringConst.
			 */
			StringConst(const std::string &value, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual StringConst &operator=(const StringConst &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the StringConst nodes are the same, do not check children.
			 */
			virtual bool operator==(const StringConst &rhs) const;

			/**
			 * Return true if the StringConst nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the StringConst nodes are the same, do not check children.
			 */
			virtual bool operator!=(const StringConst &rhs) const;

			/**
			 * Return true if the StringConst nodes are the same, do not check children.
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
			 * Return the property value.
			 */
			virtual const std::string &get_value(void) const {return m_value;}
			
			/**
			 * Change the property value.
			 */
			virtual void set_value(const std::string &value) {m_value = value;}
			

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

			
			std::string m_value;
		};

		std::ostream & operator<<(std::ostream &os, const StringConst &p);
		std::ostream & operator<<(std::ostream &os, const StringConst::Ptr p);
		


	}
}


#endif