#include <veriparse/AST/xor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Xor::Xor(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Xor);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Xor::Xor(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::Xor);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Xor &Xor::operator=(const Xor &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Xor::operator=(const Node &rhs) {
			const Xor &rhs_cast = static_cast<const Xor&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Xor::operator==(const Xor &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Xor::operator==(const Node &rhs) const {
			const Xor &rhs_cast = static_cast<const Xor&>(rhs);
			return operator==(rhs_cast);
		}

		bool Xor::operator!=(const Xor &rhs) const {
			return !(operator==(rhs));
		}

		bool Xor::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Xor::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Xor::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Xor::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Xor::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Xor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Xor::ListPtr Xor::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Xor>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Xor::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Xor::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<Xor>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Xor>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Xor::alloc_same(void) const {
			Ptr p(new Xor);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Xor &p) {
			os << "Xor: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Xor::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Xor: {nullptr}";
			}

			return os;
		}


		



	}
}