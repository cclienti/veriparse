#include <veriparse/AST/uminus.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Uminus::Uminus(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Uminus);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Uminus::Uminus(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Uminus);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Uminus &Uminus::operator=(const Uminus &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Uminus::operator=(const Node &rhs) {
			const Uminus &rhs_cast = static_cast<const Uminus&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Uminus::operator==(const Uminus &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Uminus::operator==(const Node &rhs) const {
			const Uminus &rhs_cast = static_cast<const Uminus&>(rhs);
			return operator==(rhs_cast);
		}

		bool Uminus::operator!=(const Uminus &rhs) const {
			return !(operator==(rhs));
		}

		bool Uminus::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Uminus::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Uminus::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Uminus::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Uminus::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Uminus::ListPtr Uminus::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Uminus>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Uminus::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Uminus::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Uminus>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Uminus::alloc_same(void) const {
			Ptr p(new Uminus);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Uminus &p) {
			os << "Uminus: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Uminus::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Uminus: {nullptr}";
			}

			return os;
		}


		



	}
}