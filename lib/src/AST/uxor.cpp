#include <veriparse/AST/uxor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Uxor::Uxor(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Uxor);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Uxor::Uxor(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Uxor);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Uxor &Uxor::operator=(const Uxor &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Uxor::operator=(const Node &rhs) {
			const Uxor &rhs_cast = static_cast<const Uxor&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Uxor::operator==(const Uxor &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Uxor::operator==(const Node &rhs) const {
			const Uxor &rhs_cast = static_cast<const Uxor&>(rhs);
			return operator==(rhs_cast);
		}

		bool Uxor::operator!=(const Uxor &rhs) const {
			return !(operator==(rhs));
		}

		bool Uxor::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Uxor::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Uxor::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Uxor::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Uxor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Uxor::ListPtr Uxor::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Uxor>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Uxor::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Uxor::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Uxor>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Uxor::alloc_same(void) const {
			Ptr p(new Uxor);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Uxor &p) {
			os << "Uxor: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Uxor::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Uxor: {nullptr}";
			}

			return os;
		}


		



	}
}