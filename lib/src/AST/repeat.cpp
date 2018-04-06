#include <veriparse/AST/repeat.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Repeat::Repeat(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Repeat);
			set_node_categories({NodeType::Node});
		}

		
		Repeat::Repeat(const Node::Ptr value, const Node::Ptr times, const std::string &filename, uint32_t line):
			Node(filename, line), m_value(value), m_times(times) {
			set_node_type(NodeType::Repeat);
			set_node_categories({NodeType::Node});
		}
		
		Repeat &Repeat::operator=(const Repeat &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Repeat::operator=(const Node &rhs) {
			const Repeat &rhs_cast = static_cast<const Repeat&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Repeat::operator==(const Repeat &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Repeat::operator==(const Node &rhs) const {
			const Repeat &rhs_cast = static_cast<const Repeat&>(rhs);
			return operator==(rhs_cast);
		}

		bool Repeat::operator!=(const Repeat &rhs) const {
			return !(operator==(rhs));
		}

		bool Repeat::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Repeat::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Repeat::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_value()) {
				if (get_value() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Repeat::replace matches multiple times (Node::value)";
					}
					set_value(new_node);
					found = true;
				}
			}
			if (get_times()) {
				if (get_times() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Repeat::replace matches multiple times (Node::times)";
					}
					set_times(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Repeat::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Repeat::ListPtr Repeat::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Repeat>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Repeat::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_value()) {
				list->push_back(std::static_pointer_cast<Node>(get_value()));
			}
			if (get_times()) {
				list->push_back(std::static_pointer_cast<Node>(get_times()));
			}
			return list;
		}

		void Repeat::clone_children(Node::Ptr new_node) const {
			if (get_value()) {
				cast_to<Repeat>(new_node)->set_value
					(get_value()->clone());
			}
			if (get_times()) {
				cast_to<Repeat>(new_node)->set_times
					(get_times()->clone());
			}
		}

		Node::Ptr Repeat::alloc_same(void) const {
			Ptr p(new Repeat);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Repeat &p) {
			os << "Repeat: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Repeat::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Repeat: {nullptr}";
			}

			return os;
		}


		



	}
}