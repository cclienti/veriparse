#include <veriparse/AST/partselect.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Partselect::Partselect(const std::string &filename, uint32_t line):
			Indirect(filename, line)	{
			set_node_type(NodeType::Partselect);
			set_node_categories({NodeType::Indirect, NodeType::Node});
		}

		
		Partselect::Partselect(const Node::Ptr msb, const Node::Ptr lsb, const Node::Ptr var, const std::string &filename, uint32_t line):
			Indirect(var, filename, line), m_msb(msb), m_lsb(lsb) {
			set_node_type(NodeType::Partselect);
			set_node_categories({NodeType::Indirect, NodeType::Node});
		}
		
		Partselect &Partselect::operator=(const Partselect &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Partselect::operator=(const Node &rhs) {
			const Partselect &rhs_cast = static_cast<const Partselect&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Partselect::operator==(const Partselect &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Partselect::operator==(const Node &rhs) const {
			const Partselect &rhs_cast = static_cast<const Partselect&>(rhs);
			return operator==(rhs_cast);
		}

		bool Partselect::operator!=(const Partselect &rhs) const {
			return !(operator==(rhs));
		}

		bool Partselect::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Partselect::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Partselect::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_msb()) {
				if (get_msb() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Partselect::replace matches multiple times (Node::msb)";
					}
					set_msb(new_node);
					found = true;
				}
			}
			if (get_lsb()) {
				if (get_lsb() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Partselect::replace matches multiple times (Node::lsb)";
					}
					set_lsb(new_node);
					found = true;
				}
			}
			if (get_var()) {
				if (get_var() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Partselect::replace matches multiple times (Node::var)";
					}
					set_var(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Partselect::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Partselect::ListPtr Partselect::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Partselect>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Partselect::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_msb()) {
				list->push_back(std::static_pointer_cast<Node>(get_msb()));
			}
			if (get_lsb()) {
				list->push_back(std::static_pointer_cast<Node>(get_lsb()));
			}
			if (get_var()) {
				list->push_back(std::static_pointer_cast<Node>(get_var()));
			}
			return list;
		}

		void Partselect::clone_children(Node::Ptr new_node) const {
			if (get_msb()) {
				cast_to<Partselect>(new_node)->set_msb
					(get_msb()->clone());
			}
			if (get_lsb()) {
				cast_to<Partselect>(new_node)->set_lsb
					(get_lsb()->clone());
			}
			if (get_var()) {
				cast_to<Partselect>(new_node)->set_var
					(get_var()->clone());
			}
		}

		Node::Ptr Partselect::alloc_same(void) const {
			Ptr p(new Partselect);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Partselect &p) {
			os << "Partselect: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Partselect::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Partselect: {nullptr}";
			}

			return os;
		}


		



	}
}