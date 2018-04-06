#include <veriparse/AST/pragma.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Pragma::Pragma(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Pragma);
			set_node_categories({NodeType::Node});
		}

		
		Pragma::Pragma(const Node::Ptr expression, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_expression(expression), m_name(name) {
			set_node_type(NodeType::Pragma);
			set_node_categories({NodeType::Node});
		}
		
		Pragma &Pragma::operator=(const Pragma &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &Pragma::operator=(const Node &rhs) {
			const Pragma &rhs_cast = static_cast<const Pragma&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Pragma::operator==(const Pragma &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool Pragma::operator==(const Node &rhs) const {
			const Pragma &rhs_cast = static_cast<const Pragma&>(rhs);
			return operator==(rhs_cast);
		}

		bool Pragma::operator!=(const Pragma &rhs) const {
			return !(operator==(rhs));
		}

		bool Pragma::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Pragma::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Pragma::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_expression()) {
				if (get_expression() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Pragma::replace matches multiple times (Node::expression)";
					}
					set_expression(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Pragma::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Pragma::ListPtr Pragma::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Pragma>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Pragma::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_expression()) {
				list->push_back(std::static_pointer_cast<Node>(get_expression()));
			}
			return list;
		}

		void Pragma::clone_children(Node::Ptr new_node) const {
			if (get_expression()) {
				cast_to<Pragma>(new_node)->set_expression
					(get_expression()->clone());
			}
		}

		Node::Ptr Pragma::alloc_same(void) const {
			Ptr p(new Pragma);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Pragma &p) {
			os << "Pragma: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Pragma::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Pragma: {nullptr}";
			}

			return os;
		}


		



	}
}