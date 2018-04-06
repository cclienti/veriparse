#include <veriparse/AST/intconstn.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		IntConstN::IntConstN(const std::string &filename, uint32_t line):
			Constant(filename, line)	{
			set_node_type(NodeType::IntConstN);
			set_node_categories({NodeType::Constant, NodeType::Node});
		}

		
		IntConstN::IntConstN(const int &base, const int &size, const bool &sign, const mpz_class &value, const std::string &filename, uint32_t line):
			Constant(filename, line), m_base(base), m_size(size), m_sign(sign), m_value(value) {
			set_node_type(NodeType::IntConstN);
			set_node_categories({NodeType::Constant, NodeType::Node});
		}
		
		IntConstN &IntConstN::operator=(const IntConstN &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_base(rhs.get_base());
			set_size(rhs.get_size());
			set_sign(rhs.get_sign());
			set_value(rhs.get_value());
			return *this;
		}

		Node &IntConstN::operator=(const Node &rhs) {
			const IntConstN &rhs_cast = static_cast<const IntConstN&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool IntConstN::operator==(const IntConstN &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_base() != rhs.get_base()) {
				return false;
			}
			if (get_size() != rhs.get_size()) {
				return false;
			}
			if (get_sign() != rhs.get_sign()) {
				return false;
			}
			if (get_value() != rhs.get_value()) {
				return false;
			}
			return true;
		}

		bool IntConstN::operator==(const Node &rhs) const {
			const IntConstN &rhs_cast = static_cast<const IntConstN&>(rhs);
			return operator==(rhs_cast);
		}

		bool IntConstN::operator!=(const IntConstN &rhs) const {
			return !(operator==(rhs));
		}

		bool IntConstN::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool IntConstN::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool IntConstN::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			return found;
		}

		bool IntConstN::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		IntConstN::ListPtr IntConstN::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<IntConstN>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr IntConstN::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			return list;
		}

		void IntConstN::clone_children(Node::Ptr new_node) const {
		}

		Node::Ptr IntConstN::alloc_same(void) const {
			Ptr p(new IntConstN);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const IntConstN &p) {
			os << "IntConstN: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "base: " << p.get_base()<< ", ";
			
			os << "size: " << p.get_size()<< ", ";
			
			os << "sign: " << p.get_sign()<< ", ";
			
			os << "value: " << p.get_value();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const IntConstN::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "IntConstN: {nullptr}";
			}

			return os;
		}


		



	}
}