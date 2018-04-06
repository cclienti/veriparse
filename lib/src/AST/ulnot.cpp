#include <veriparse/AST/ulnot.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Ulnot::Ulnot(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Ulnot);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Ulnot::Ulnot(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Ulnot);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Ulnot &Ulnot::operator=(const Ulnot &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Ulnot::operator=(const Node &rhs) {
			const Ulnot &rhs_cast = static_cast<const Ulnot&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Ulnot::operator==(const Ulnot &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Ulnot::operator==(const Node &rhs) const {
			const Ulnot &rhs_cast = static_cast<const Ulnot&>(rhs);
			return operator==(rhs_cast);
		}

		bool Ulnot::operator!=(const Ulnot &rhs) const {
			return !(operator==(rhs));
		}

		bool Ulnot::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Ulnot::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Ulnot::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Ulnot::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Ulnot::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Ulnot::ListPtr Ulnot::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Ulnot>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Ulnot::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Ulnot::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Ulnot>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Ulnot::alloc_same(void) const {
			Ptr p(new Ulnot);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Ulnot &p) {
			os << "Ulnot: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Ulnot::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Ulnot: {nullptr}";
			}

			return os;
		}


		



	}
}