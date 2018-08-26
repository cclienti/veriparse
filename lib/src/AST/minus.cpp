#include <veriparse/AST/minus.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Minus::Minus(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Minus);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Minus::Minus(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::Minus);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Minus &Minus::operator=(const Minus &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Minus::operator=(const Node &rhs) {
			const Minus &rhs_cast = static_cast<const Minus&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Minus::operator==(const Minus &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Minus::operator==(const Node &rhs) const {
			const Minus &rhs_cast = static_cast<const Minus&>(rhs);
			return operator==(rhs_cast);
		}

		bool Minus::operator!=(const Minus &rhs) const {
			return !(operator==(rhs));
		}

		bool Minus::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Minus::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Minus::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Minus::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Minus::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Minus::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Minus::ListPtr Minus::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Minus>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Minus::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Minus::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<Minus>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Minus>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Minus::alloc_same(void) const {
			Ptr p(new Minus);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Minus &p) {
			os << "Minus: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Minus::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Minus: {nullptr}";
			}

			return os;
		}


		



	}
}