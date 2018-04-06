#ifndef VERIPARSE_AST_REG_HPP
#define VERIPARSE_AST_REG_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/variable.hpp>

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

		class Reg: public Variable {
		public:
			using Ptr = typename NodePointers<Reg>::Ptr;
			using List = typename NodePointers<Reg>::List;
			using ListPtr = typename NodePointers<Reg>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Reg.
			 */
			Reg(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Reg.
			 */
			Reg(const Width::ListPtr widths, const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign, const std::string &name, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Reg &operator=(const Reg &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Reg nodes are the same, do not check children.
			 */
			virtual bool operator==(const Reg &rhs) const;

			/**
			 * Return true if the Reg nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Reg nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Reg &rhs) const;

			/**
			 * Return true if the Reg nodes are the same, do not check children.
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
			 * Return the child widths.
			 */
			virtual Width::ListPtr get_widths(void) const {return m_widths;}
			
			/**
			 * Change the child widths.
			 */
			virtual void set_widths(Width::ListPtr widths) {
				m_widths = widths;
			}
			
			/**
			 * Return the property sign.
			 */
			virtual const bool &get_sign(void) const {return m_sign;}
			
			/**
			 * Change the property sign.
			 */
			virtual void set_sign(const bool &sign) {m_sign = sign;}
			

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

			
			Width::ListPtr m_widths;
			bool m_sign;
		};

		std::ostream & operator<<(std::ostream &os, const Reg &p);
		std::ostream & operator<<(std::ostream &os, const Reg::Ptr p);
		


	}
}


#endif