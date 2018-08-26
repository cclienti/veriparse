#include <veriparse/AST/sra.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Sra::Sra(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Sra);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Sra::Sra(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line) {
			set_node_type(NodeType::Sra);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Sra &Sra::operator=(const Sra &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Sra::operator=(const Node &rhs) {
			const Sra &rhs_cast = static_cast<const Sra&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Sra::operator==(const Sra &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Sra::operator==(const Node &rhs) const {
			const Sra &rhs_cast = static_cast<const Sra&>(rhs);
			return operator==(rhs_cast);
		}

		bool Sra::operator!=(const Sra &rhs) const {
			return !(operator==(rhs));
		}

		bool Sra::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Sra::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Sra::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Sra::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Sra::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Sra::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Sra::ListPtr Sra::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Sra>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Sra::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Sra::clone_children(Node::Ptr new_node) const {
			if (get_left()) {
				cast_to<Sra>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Sra>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Sra::alloc_same(void) const {
			Ptr p(new Sra);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Sra &p) {
			os << "Sra: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Sra::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Sra: {nullptr}";
			}

			return os;
		}


		



	}
}