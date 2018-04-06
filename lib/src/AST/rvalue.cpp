#include <veriparse/AST/rvalue.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Rvalue::Rvalue(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Rvalue);
			set_node_categories({NodeType::Node});
		}

		
		Rvalue::Rvalue(const Node::Ptr var, const std::string &filename, uint32_t line):
			Node(filename, line), m_var(var) {
			set_node_type(NodeType::Rvalue);
			set_node_categories({NodeType::Node});
		}
		
		Rvalue &Rvalue::operator=(const Rvalue &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Rvalue::operator=(const Node &rhs) {
			const Rvalue &rhs_cast = static_cast<const Rvalue&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Rvalue::operator==(const Rvalue &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Rvalue::operator==(const Node &rhs) const {
			const Rvalue &rhs_cast = static_cast<const Rvalue&>(rhs);
			return operator==(rhs_cast);
		}

		bool Rvalue::operator!=(const Rvalue &rhs) const {
			return !(operator==(rhs));
		}

		bool Rvalue::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Rvalue::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Rvalue::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_var()) {
				if (get_var() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Rvalue::replace matches multiple times (Node::var)";
					}
					set_var(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Rvalue::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Rvalue::ListPtr Rvalue::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Rvalue>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Rvalue::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_var()) {
				list->push_back(std::static_pointer_cast<Node>(get_var()));
			}
			return list;
		}

		void Rvalue::clone_children(Node::Ptr new_node) const {
			if (get_var()) {
				cast_to<Rvalue>(new_node)->set_var
					(get_var()->clone());
			}
		}

		Node::Ptr Rvalue::alloc_same(void) const {
			Ptr p(new Rvalue);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Rvalue &p) {
			os << "Rvalue: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Rvalue::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Rvalue: {nullptr}";
			}

			return os;
		}


		



	}
}