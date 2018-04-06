#include <veriparse/AST/unot.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Unot::Unot(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Unot);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Unot::Unot(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Unot);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Unot &Unot::operator=(const Unot &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Unot::operator=(const Node &rhs) {
			const Unot &rhs_cast = static_cast<const Unot&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Unot::operator==(const Unot &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Unot::operator==(const Node &rhs) const {
			const Unot &rhs_cast = static_cast<const Unot&>(rhs);
			return operator==(rhs_cast);
		}

		bool Unot::operator!=(const Unot &rhs) const {
			return !(operator==(rhs));
		}

		bool Unot::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Unot::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Unot::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Unot::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Unot::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Unot::ListPtr Unot::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Unot>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Unot::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Unot::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Unot>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Unot::alloc_same(void) const {
			Ptr p(new Unot);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Unot &p) {
			os << "Unot: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Unot::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Unot: {nullptr}";
			}

			return os;
		}


		



	}
}