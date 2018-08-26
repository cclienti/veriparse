#include <veriparse/AST/unaryoperator.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		UnaryOperator::UnaryOperator(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::UnaryOperator);
			set_node_categories({NodeType::Node});
		}

		
		UnaryOperator::UnaryOperator(const Node::Ptr right, const std::string &filename, uint32_t line):
			Node(filename, line), m_right(right) {
			set_node_type(NodeType::UnaryOperator);
			set_node_categories({NodeType::Node});
		}
		
		UnaryOperator &UnaryOperator::operator=(const UnaryOperator &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &UnaryOperator::operator=(const Node &rhs) {
			const UnaryOperator &rhs_cast = static_cast<const UnaryOperator&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool UnaryOperator::operator==(const UnaryOperator &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool UnaryOperator::operator==(const Node &rhs) const {
			const UnaryOperator &rhs_cast = static_cast<const UnaryOperator&>(rhs);
			return operator==(rhs_cast);
		}

		bool UnaryOperator::operator!=(const UnaryOperator &rhs) const {
			return !(operator==(rhs));
		}

		bool UnaryOperator::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool UnaryOperator::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool UnaryOperator::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "UnaryOperator::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool UnaryOperator::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		UnaryOperator::ListPtr UnaryOperator::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<UnaryOperator>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr UnaryOperator::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void UnaryOperator::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<UnaryOperator>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr UnaryOperator::alloc_same(void) const {
			Ptr p(new UnaryOperator);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const UnaryOperator &p) {
			os << "UnaryOperator: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const UnaryOperator::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "UnaryOperator: {nullptr}";
			}

			return os;
		}


		



	}
}