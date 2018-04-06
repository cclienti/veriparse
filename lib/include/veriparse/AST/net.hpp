#ifndef VERIPARSE_AST_NET_HPP
#define VERIPARSE_AST_NET_HPP

#include <veriparse/AST/node.hpp>
#include <veriparse/AST/variable.hpp>

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

		class Net: public Variable {
		public:
			using Ptr = typename NodePointers<Net>::Ptr;
			using List = typename NodePointers<Net>::List;
			using ListPtr = typename NodePointers<Net>::ListPtr;

			/**
			 * Constructor, m_node_type is set to NodeType::Net.
			 */
			Net(const std::string &filename="", uint32_t line=0);
			
			/**
			 * Constructor, m_node_type is set to NodeType::Net.
			 */
			Net(const Width::ListPtr widths, const DelayStatement::Ptr ldelay, const DelayStatement::Ptr rdelay, const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign, const std::string &name, const std::string &filename="", uint32_t line=0);
			

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Net &operator=(const Net &rhs);

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &rhs) override;

			/**
			 * Return true if the Net nodes are the same, do not check children.
			 */
			virtual bool operator==(const Net &rhs) const;

			/**
			 * Return true if the Net nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const override;

			/**
			 * Return true if the Net nodes are the same, do not check children.
			 */
			virtual bool operator!=(const Net &rhs) const;

			/**
			 * Return true if the Net nodes are the same, do not check children.
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
			 * Return the child ldelay.
			 */
			virtual DelayStatement::Ptr get_ldelay(void) const {return m_ldelay;}
			
			/**
			 * Return the child rdelay.
			 */
			virtual DelayStatement::Ptr get_rdelay(void) const {return m_rdelay;}
			
			/**
			 * Change the child widths.
			 */
			virtual void set_widths(Width::ListPtr widths) {
				m_widths = widths;
			}
			
			/**
			 * Change the child ldelay.
			 */
			virtual void set_ldelay(DelayStatement::Ptr ldelay) {
				m_ldelay = ldelay;
			}
			
			/**
			 * Change the child rdelay.
			 */
			virtual void set_rdelay(DelayStatement::Ptr rdelay) {
				m_rdelay = rdelay;
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
			DelayStatement::Ptr m_ldelay;
			DelayStatement::Ptr m_rdelay;
			bool m_sign;
		};

		std::ostream & operator<<(std::ostream &os, const Net &p);
		std::ostream & operator<<(std::ostream &os, const Net::Ptr p);
		


	}
}


#endif