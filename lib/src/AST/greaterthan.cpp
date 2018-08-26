#include <veriparse/AST/greaterthan.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		GreaterThan::GreaterThan(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::GreaterThan);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		GreaterThan::GreaterThan(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::GreaterThan);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		GreaterThan &GreaterThan::operator=(const GreaterThan &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &GreaterThan::operator=(const Node &rhs) {
			const GreaterThan &rhs_cast = static_cast<const GreaterThan&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool GreaterThan::operator==(const GreaterThan &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool GreaterThan::operator==(const Node &rhs) const {
			const GreaterThan &rhs_cast = static_cast<const GreaterThan&>(rhs);
			return operator==(rhs_cast);
		}

		bool GreaterThan::operator!=(const GreaterThan &rhs) const {
			return !(operator==(rhs));
		}

		bool GreaterThan::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool GreaterThan::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool GreaterThan::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "GreaterThan::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "GreaterThan::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool GreaterThan::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		GreaterThan::ListPtr GreaterThan::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<GreaterThan>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr GreaterThan::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void GreaterThan::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<GreaterThan>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<GreaterThan>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr GreaterThan::alloc_same(void) const {
			Ptr p(new GreaterThan);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const GreaterThan &p) {
			os << "GreaterThan: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const GreaterThan::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "GreaterThan: {nullptr}";
			}

			return os;
		}


		



	}
}