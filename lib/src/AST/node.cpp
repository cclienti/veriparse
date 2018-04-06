#include <veriparse/AST/node.hpp>
#include <veriparse/AST/node_ostream.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>
#include <vector>


namespace Veriparse {
	namespace AST {

		Node::Node(const std::string &filename, uint32_t line):
			m_node_type(NodeType::Node),
			m_filename(filename),
			m_line(line) {
		}

		Node::~Node(void) {
		}

		bool Node::is_node_category(NodeType node_type) const {
			if(is_node_type(node_type)) return true;
			const std::vector<NodeType> &v = get_node_categories();
			return std::find(v.begin(), v.end(), node_type) != v.end();
		}

		Node &Node::operator=(const Node &that) {
			m_filename = that.m_filename;
			m_line = that.m_line;
			return *this;
		}

		bool Node::operator==(const Node &rhs) const {
			if (get_node_type() == rhs.get_node_type()) {
				return true;
			}
			else {
				return false;
			}
		}

		bool Node::operator!=(const Node &rhs) const {
			return !(*this == rhs);
		}

		bool Node::remove(Node::Ptr node) {
			return false;
		}

		bool Node::replace(Node::Ptr node, Node::Ptr new_node) {
			return false;
		}

		bool Node::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			return false;
		}

		Node::Ptr Node::clone(void) const {
			Ptr new_node = alloc_same();
			*new_node = *this;

			clone_children(new_node);

			return new_node;
		}

		bool Node::contains(NodeType node_type) const {
			if (get_node_type() == node_type) return true;

			Node::ListPtr children = get_children();
			for (Ptr node: *children) {
				if(node->contains(node_type)) return true;
			}

			return false;
		}


		bool Node::is_equal(const Node &node, bool full_check) const {
			// Check for same pointer
			if (&node == this) {
				if (full_check) {
					LOG_ERROR << "Node::is_equal: Pointers are the same";
					return false;
				}
				else {
					LOG_WARNING << "Node::is_equal: Pointers are the same";
				}
			}

			// Check if nodes are equals (no recursion)
			if (!(node == *this)) {
				LOG_DEBUG << "Node::is_equal: node parameters are different (this) \"" << *this << "\""
				          << " and (node) \"" << node << "\"";
				return false;
			}

			// If full_check is true, we check if line number and file
			// names are the same
			if (full_check) {
				if ((node.get_filename() != get_filename()) || (node.get_line() != get_line())) {
					LOG_DEBUG << "Node::is_equal: filename or line number is different: (this) \""
					          << get_filename() << ":" << get_line() << "\""
					          << " and (node) \"" << node.get_filename() << ":" << node.get_line() << "\"";
					return false;
				}
			}

			// Check list content
			Node::ListPtr node_children1 = node.get_children();
			Node::ListPtr node_children2 = get_children();

			List::const_iterator it1 = node_children1->begin();
			List::const_iterator it2 = node_children2->begin();

			for(; (it1 != node_children1->end()) && (it2 != node_children2->end()); ++it1, ++it2) {
				const Node &ref1 = **it1;
				const Node &ref2 = **it2;
				LOG_TRACE << "Node::is_equal: check children: checking in \"this\", type: "
				          << NodeTypeToString((*it2)->get_node_type());
				LOG_TRACE << "Node::is_equal: check children: checking in node, type: "
				          << NodeTypeToString((*it1)->get_node_type());

				if (!(ref2.is_equal(ref1, full_check))) {
					// Child are different
					LOG_DEBUG << "Node::is_equal: check children: diff between \""
					          << ref2.get_filename() << ":" << ref2.get_line() << "\""
					          << " and \"" << ref1.get_filename() << ":" << ref1.get_line() << "\"";
					return false;
				}
			}

			if ((it1 == node_children1->end()) && (it2 == node_children2->end())) {
				// Children are the same
				return true;
			}
			else {
				// The size of list differs
				LOG_DEBUG << "Node::is_equal: check children: number of children are not the same."
				          << " \"this\" children list size is " << node_children2->size()
				          << " and node children list size is " << node_children1->size();
				for(; it1 != node_children1->end(); ++it1) {
					LOG_DEBUG << "Node::is_equal: check children: remaining not checked child in the list in the node: "
					          << *it1;
				}
				for(; it2 != node_children2->end(); ++it2) {
					LOG_DEBUG << "Node::is_equal: check children: remaining not checked child in the list in \"this\": "
					          << *it2;
				}
				return false;
			}
		}

		bool Node::is_equal(const Ptr node, bool full_check) const {
			if(node) {
				return is_equal(*node, full_check);
			}

			return false;
		}

		bool Node::is_not_equal(const Node &node, bool full_check) const {
			return !is_equal(node, full_check);
		}

		bool Node::is_not_equal(const Ptr node, bool full_check) const {
			if(node) {
				return is_not_equal(*node, full_check);
			}

			return false;
		}

		Node::ListPtr Node::get_children(void) const {
			return std::make_shared<Node::List>();
		}

		Node::ListPtr Node::clone_list(const ListPtr nodes)	{
			if (nodes) {
				ListPtr list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					if (p) {
						list->push_back(p->clone());
					}
				}
				return list;
			}
			return ListPtr(nullptr);
		}

		Node::Ptr Node::alloc_same(void) const {
		 	Ptr p(new Node);
		 	return p;
		}

		void Node::clone_children(Ptr new_node) const {}


	}
}
