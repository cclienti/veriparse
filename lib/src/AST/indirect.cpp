#include <veriparse/AST/indirect.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Indirect::Indirect(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Indirect);
			set_node_categories({NodeType::Node});
		}

		
		Indirect::Indirect(const Node::Ptr var, const std::string &filename, uint32_t line):
			Node(filename, line), m_var(var) {
			set_node_type(NodeType::Indirect);
			set_node_categories({NodeType::Node});
		}
		
		Indirect &Indirect::operator=(const Indirect &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Indirect::operator=(const Node &rhs) {
			const Indirect &rhs_cast = static_cast<const Indirect&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Indirect::operator==(const Indirect &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Indirect::operator==(const Node &rhs) const {
			const Indirect &rhs_cast = static_cast<const Indirect&>(rhs);
			return operator==(rhs_cast);
		}

		bool Indirect::operator!=(const Indirect &rhs) const {
			return !(operator==(rhs));
		}

		bool Indirect::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Indirect::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Indirect::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_var()) {
				if (get_var() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Indirect::replace matches multiple times (Node::var)";
					}
					set_var(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Indirect::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Indirect::ListPtr Indirect::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Indirect>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Indirect::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_var()) {
				list->push_back(std::static_pointer_cast<Node>(get_var()));
			}
			return list;
		}

		void Indirect::clone_children(Node::Ptr new_node) const {
			if (get_var()) {
				cast_to<Indirect>(new_node)->set_var
					(get_var()->clone());
			}
		}

		Node::Ptr Indirect::alloc_same(void) const {
			Ptr p(new Indirect);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Indirect &p) {
			os << "Indirect: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Indirect::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Indirect: {nullptr}";
			}

			return os;
		}


		



	}
}