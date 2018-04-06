#include <veriparse/AST/divide.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Divide::Divide(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Divide);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Divide::Divide(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::Divide);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Divide &Divide::operator=(const Divide &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Divide::operator=(const Node &rhs) {
			const Divide &rhs_cast = static_cast<const Divide&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Divide::operator==(const Divide &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Divide::operator==(const Node &rhs) const {
			const Divide &rhs_cast = static_cast<const Divide&>(rhs);
			return operator==(rhs_cast);
		}

		bool Divide::operator!=(const Divide &rhs) const {
			return !(operator==(rhs));
		}

		bool Divide::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Divide::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Divide::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Divide::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Divide::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Divide::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Divide::ListPtr Divide::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Divide>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Divide::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Divide::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<Divide>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Divide>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Divide::alloc_same(void) const {
			Ptr p(new Divide);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Divide &p) {
			os << "Divide: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Divide::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Divide: {nullptr}";
			}

			return os;
		}


		



	}
}