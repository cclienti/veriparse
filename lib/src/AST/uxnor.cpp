#include <veriparse/AST/uxnor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Uxnor::Uxnor(const std::string &filename, uint32_t line):
			UnaryOperator(filename, line)	{
			set_node_type(NodeType::Uxnor);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}

		
		Uxnor::Uxnor(const Node::Ptr right, const std::string &filename, uint32_t line):
			UnaryOperator(right, filename, line) {
			set_node_type(NodeType::Uxnor);
			set_node_categories({NodeType::UnaryOperator, NodeType::Node});
		}
		
		Uxnor &Uxnor::operator=(const Uxnor &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Uxnor::operator=(const Node &rhs) {
			const Uxnor &rhs_cast = static_cast<const Uxnor&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Uxnor::operator==(const Uxnor &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Uxnor::operator==(const Node &rhs) const {
			const Uxnor &rhs_cast = static_cast<const Uxnor&>(rhs);
			return operator==(rhs_cast);
		}

		bool Uxnor::operator!=(const Uxnor &rhs) const {
			return !(operator==(rhs));
		}

		bool Uxnor::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Uxnor::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Uxnor::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Uxnor::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Uxnor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Uxnor::ListPtr Uxnor::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Uxnor>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Uxnor::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Uxnor::clone_children(Node::Ptr new_node) const {
			if (get_right()) {
				cast_to<Uxnor>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Uxnor::alloc_same(void) const {
			Ptr p(new Uxnor);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Uxnor &p) {
			os << "Uxnor: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Uxnor::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Uxnor: {nullptr}";
			}

			return os;
		}


		



	}
}