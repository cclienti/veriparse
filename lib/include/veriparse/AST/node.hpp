#ifndef VERIPARSE_AST_NODE
#define VERIPARSE_AST_NODE

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <vector>
#include <iostream>

#include <veriparse/AST/node_type.hpp>
#include <veriparse/AST/node_pointers.hpp>


namespace Veriparse {
	namespace AST {

		class Node {
		public:
			using Ptr = typename NodePointers<Node>::Ptr;
			using List = typename NodePointers<Node>::List;
			using ListPtr = typename NodePointers<Node>::ListPtr;

		public:
			/**
			 * Constructor, m_node_type remains set to
			 * NodeType::None.
			 */
			Node(const std::string &filename="", uint32_t line=0);

			/**
			 * No copy constructor.
			 */
			Node(const Node &that) = delete;

			/**
			 * Destroy the node and all children (recursive).
			 */
			virtual ~Node(void);

			/**
			 * Return the type of the node.
			 */
			virtual NodeType get_node_type(void) const {
				return m_node_type;
			}

			/**
			 * Return true if the given node type corresponds to the type
			 * of the node.
			 */
			virtual bool is_node_type(NodeType node_type) const {
				return m_node_type == node_type;
			}

			/**
			 * Return true if the given node category corresponds to the
			 * category of the node.
			 */
			virtual const std::vector<NodeType> &get_node_categories() const {
				return m_node_categories;
			}

			/**
			 * Return true if the given node category corresponds to the
			 * category of the node.
			 */
			virtual bool is_node_category(NodeType node_type) const;

			/**
			 * Return the filename where the node was built.
			 */
			virtual const std::string &get_filename(void) const {return m_filename;}

			/**
			 * Return the line number where the node was built. If zero,
			 * the information is not available.
			 */
			virtual int32_t get_line(void) const {return m_line;}

			/**
			 * Set the filename.
			 */
			virtual void set_filename(std::string filename) {m_filename = filename;}

			/**
			 * Set the line number.
			 */
			virtual void set_line(uint32_t line) {m_line = line;}

			/**
			 * Return true if the nodes and all children are equals
			 * (recursive). If full_check is true, m_line and m_filename
			 * will also be checked.
			 */
			virtual bool is_equal(const Node &node, bool full_check=false) const;

			/**
			 * Return true if the nodes and all children are equals
			 * (recursive). If full_check is true, m_line and m_filename
			 * will also be checked.
			 */
			virtual bool is_equal(const Ptr node, bool full_check=false) const;

			/**
			 * Return true if the nodes are diffirent or if couple of
			 * children are differents (recursive). If full_check is
			 * true, m_line and m_filename will also be checked.
			 */
			virtual bool is_not_equal(const Node &node, bool full_check=false) const;

			/**
			 * Return true if the nodes are diffirent or if couple of
			 * children are different (recursive). If full_check is true,
			 * m_line and m_filename will also be checked.
			 */
			virtual bool is_not_equal(const Ptr node, bool full_check=false) const;

			/**
			 * Assignment operator, do not affect children.
			 */
			virtual Node &operator=(const Node &that);

			/**
			 * Return true if the nodes are the same, do not check children.
			 */
			virtual bool operator==(const Node &rhs) const;

			/**
			 * Return true if the nodes are differents, do not check children.
			 */
			virtual bool operator!=(const Node &rhs) const;

			/**
			 * Return a list of all children nodes.
			 */
			virtual ListPtr get_children(void) const;

			/**
			 * Return true if the current node or its children (and
			 * recursively) contains the given node type.
			 */
			virtual bool contains(NodeType node_type) const;

			/**
			 * Remove the given children.
			 */
			virtual bool remove(Ptr node);

			/**
			 * Replace the given children node by the new node.
			 */
			virtual bool replace(Ptr node, Ptr new_node);

			/**
			 * Replace the given children node by the new node.
			 */
			virtual bool replace(Ptr node, ListPtr new_nodes);

			/**
			 * Return a clone of the current Node and of all children
			 * (recursive).
			 */
			virtual Ptr clone(void) const;

			/**
			 * Return a clone of the current List and of all children
			 * (recursive).
			 */
			static ListPtr clone_list(const ListPtr nodes);


		protected:
			/**
			 * Set the type of the node.
			 */
			void set_node_type(NodeType node_type) {
				m_node_type = node_type;
			}

			/**
			 * Set the category of the node.
			 */
			void set_node_categories(const std::vector<NodeType> &node_categories) {
				m_node_categories = node_categories;
			}

			/**
			 * Clone the instance children and attach them to the
			 * new_node. This method is called by Node::clone().
			 */
			virtual void clone_children(Ptr new_node) const;

		private:
			/**
			 * Allocate a new node with the same node type than the current instance.
			 */
			virtual Ptr alloc_same(void) const;

			NodeType              m_node_type;
			std::vector<NodeType> m_node_categories;
			std::string           m_filename;
			uint32_t              m_line;
		};

	}
}

#endif
