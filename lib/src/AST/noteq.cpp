#include <veriparse/AST/noteq.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		NotEq::NotEq(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::NotEq);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		NotEq::NotEq(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::NotEq);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		NotEq &NotEq::operator=(const NotEq &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &NotEq::operator=(const Node &rhs) {
			const NotEq &rhs_cast = static_cast<const NotEq&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool NotEq::operator==(const NotEq &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool NotEq::operator==(const Node &rhs) const {
			const NotEq &rhs_cast = static_cast<const NotEq&>(rhs);
			return operator==(rhs_cast);
		}

		bool NotEq::operator!=(const NotEq &rhs) const {
			return !(operator==(rhs));
		}

		bool NotEq::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool NotEq::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool NotEq::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "NotEq::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "NotEq::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool NotEq::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		NotEq::ListPtr NotEq::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<NotEq>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr NotEq::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void NotEq::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<NotEq>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<NotEq>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr NotEq::alloc_same(void) const {
			Ptr p(new NotEq);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const NotEq &p) {
			os << "NotEq: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const NotEq::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "NotEq: {nullptr}";
			}

			return os;
		}


		



	}
}