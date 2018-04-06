#include <veriparse/AST/ioport.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Ioport::Ioport(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Ioport);
			set_node_categories({NodeType::Node});
		}

		
		Ioport::Ioport(const IODir::Ptr first, const Variable::Ptr second, const std::string &filename, uint32_t line):
			Node(filename, line), m_first(first), m_second(second) {
			set_node_type(NodeType::Ioport);
			set_node_categories({NodeType::Node});
		}
		
		Ioport &Ioport::operator=(const Ioport &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Ioport::operator=(const Node &rhs) {
			const Ioport &rhs_cast = static_cast<const Ioport&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Ioport::operator==(const Ioport &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Ioport::operator==(const Node &rhs) const {
			const Ioport &rhs_cast = static_cast<const Ioport&>(rhs);
			return operator==(rhs_cast);
		}

		bool Ioport::operator!=(const Ioport &rhs) const {
			return !(operator==(rhs));
		}

		bool Ioport::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Ioport::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Ioport::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_first()) {
				if (get_first() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Ioport::replace matches multiple times (IODir::first)";
					}
					set_first(cast_to<IODir>(new_node));
					found = true;
				}
			}
			if (get_second()) {
				if (get_second() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Ioport::replace matches multiple times (Variable::second)";
					}
					set_second(cast_to<Variable>(new_node));
					found = true;
				}
			}
			return found;
		}

		bool Ioport::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Ioport::ListPtr Ioport::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Ioport>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Ioport::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_first()) {
				list->push_back(std::static_pointer_cast<Node>(get_first()));
			}
			if (get_second()) {
				list->push_back(std::static_pointer_cast<Node>(get_second()));
			}
			return list;
		}

		void Ioport::clone_children(Node::Ptr new_node) const {
			if (get_first()) {
				cast_to<Ioport>(new_node)->set_first
					(cast_to<IODir>(get_first()->clone()));
			}
			if (get_second()) {
				cast_to<Ioport>(new_node)->set_second
					(cast_to<Variable>(get_second()->clone()));
			}
		}

		Node::Ptr Ioport::alloc_same(void) const {
			Ptr p(new Ioport);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Ioport &p) {
			os << "Ioport: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Ioport::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Ioport: {nullptr}";
			}

			return os;
		}


		



	}
}