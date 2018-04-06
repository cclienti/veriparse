#include <veriparse/AST/unand.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Unand::Unand(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Unand);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Unand::Unand(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Unand);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Unand &Unand::operator=(const Unand &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Unand::operator=(const Node &rhs) {
			const Unand &rhs_cast = static_cast<const Unand&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Unand::operator==(const Unand &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Unand::operator==(const Node &rhs) const {
			const Unand &rhs_cast = static_cast<const Unand&>(rhs);
			return operator==(rhs_cast);
		}

		bool Unand::operator!=(const Unand &rhs) const {
			return !(operator==(rhs));
		}

		bool Unand::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Unand::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Unand::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Unand::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Unand::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Unand::ListPtr Unand::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Unand>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Unand::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Unand::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Unand>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Unand::alloc_same(void) const {
			Ptr p(new Unand);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Unand &p) {
			os << "Unand: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Unand::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Unand: {nullptr}";
			}

			return os;
		}


		



	}
}