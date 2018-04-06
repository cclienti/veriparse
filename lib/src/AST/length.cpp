#include <veriparse/AST/length.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Length::Length(const std::string &filename, uint32_t line):
			Width(filename, line)	{
			set_node_type(NodeType::Length);
			set_node_categories({NodeType::Width, NodeType::Node});
		}

		
		Length::Length(const Node::Ptr msb, const Node::Ptr lsb, const std::string &filename, uint32_t line):
			Width(msb, lsb, filename, line) {
			set_node_type(NodeType::Length);
			set_node_categories({NodeType::Width, NodeType::Node});
		}
		
		Length &Length::operator=(const Length &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Length::operator=(const Node &rhs) {
			const Length &rhs_cast = static_cast<const Length&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Length::operator==(const Length &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Length::operator==(const Node &rhs) const {
			const Length &rhs_cast = static_cast<const Length&>(rhs);
			return operator==(rhs_cast);
		}

		bool Length::operator!=(const Length &rhs) const {
			return !(operator==(rhs));
		}

		bool Length::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Length::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Length::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_msb()) {
				if (get_msb() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Length::replace matches multiple times (Node::msb)";
					}
					set_msb(new_node);
					found = true;
				}
			}
			if (get_lsb()) {
				if (get_lsb() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Length::replace matches multiple times (Node::lsb)";
					}
					set_lsb(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Length::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Length::ListPtr Length::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Length>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Length::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_msb()) {
				list->push_back(std::static_pointer_cast<Node>(get_msb()));
			}
			if (get_lsb()) {
				list->push_back(std::static_pointer_cast<Node>(get_lsb()));
			}
			return list;
		}

		void Length::clone_children(Node::Ptr new_node) const {
			if (get_msb()) {
				cast_to<Length>(new_node)->set_msb
					(get_msb()->clone());
			}
			if (get_lsb()) {
				cast_to<Length>(new_node)->set_lsb
					(get_lsb()->clone());
			}
		}

		Node::Ptr Length::alloc_same(void) const {
			Ptr p(new Length);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Length &p) {
			os << "Length: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Length::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Length: {nullptr}";
			}

			return os;
		}


		



	}
}