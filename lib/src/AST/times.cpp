#include <veriparse/AST/times.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Times::Times(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Times);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Times::Times(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::Times);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Times &Times::operator=(const Times &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Times::operator=(const Node &rhs) {
			const Times &rhs_cast = static_cast<const Times&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Times::operator==(const Times &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Times::operator==(const Node &rhs) const {
			const Times &rhs_cast = static_cast<const Times&>(rhs);
			return operator==(rhs_cast);
		}

		bool Times::operator!=(const Times &rhs) const {
			return !(operator==(rhs));
		}

		bool Times::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Times::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Times::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Times::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Times::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Times::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Times::ListPtr Times::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Times>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Times::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Times::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<Times>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Times>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Times::alloc_same(void) const {
			Ptr p(new Times);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Times &p) {
			os << "Times: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Times::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Times: {nullptr}";
			}

			return os;
		}


		



	}
}