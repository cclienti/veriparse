#include <veriparse/AST/cond.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Cond::Cond(const std::string &filename, uint32_t line):
			Operator(filename, line)	{
			set_node_type(NodeType::Cond);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}

		
		Cond::Cond(const Node::Ptr cond, const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
			Operator(left, right, filename, line), m_cond(cond) {
			set_node_type(NodeType::Cond);
			set_node_categories({NodeType::Operator, NodeType::Node});
		}
		
		Cond &Cond::operator=(const Cond &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Cond::operator=(const Node &rhs) {
			const Cond &rhs_cast = static_cast<const Cond&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Cond::operator==(const Cond &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Cond::operator==(const Node &rhs) const {
			const Cond &rhs_cast = static_cast<const Cond&>(rhs);
			return operator==(rhs_cast);
		}

		bool Cond::operator!=(const Cond &rhs) const {
			return !(operator==(rhs));
		}

		bool Cond::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Cond::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Cond::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_cond()) {
				if (get_cond() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Cond::replace matches multiple times (Node::cond)";
					}
					set_cond(new_node);
					found = true;
				}
			}
			if (get_left()) {
				if (get_left() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Cond::replace matches multiple times (Node::left)";
					}
					set_left(new_node);
					found = true;
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Cond::replace matches multiple times (Node::right)";
					}
					set_right(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Cond::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Cond::ListPtr Cond::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Cond>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Cond::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_cond()) {
				list->push_back(std::static_pointer_cast<Node>(get_cond()));
			}
			if (get_left()) {
				list->push_back(std::static_pointer_cast<Node>(get_left()));
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Cond::clone_children(Node::Ptr new_node) const {
			if (get_cond()) {
				cast_to<Cond>(new_node)->set_cond
					(get_cond()->clone());
			}
			if (get_left()) {
				cast_to<Cond>(new_node)->set_left
					(get_left()->clone());
			}
			if (get_right()) {
				cast_to<Cond>(new_node)->set_right
					(get_right()->clone());
			}
		}

		Node::Ptr Cond::alloc_same(void) const {
			Ptr p(new Cond);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Cond &p) {
			os << "Cond: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Cond::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Cond: {nullptr}";
			}

			return os;
		}


		



	}
}