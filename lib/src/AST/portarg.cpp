#include <veriparse/AST/portarg.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		PortArg::PortArg(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::PortArg);
			set_node_categories({NodeType::Node});
		}

		
		PortArg::PortArg(const Node::Ptr value, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_value(value), m_name(name) {
			set_node_type(NodeType::PortArg);
			set_node_categories({NodeType::Node});
		}
		
		PortArg &PortArg::operator=(const PortArg &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &PortArg::operator=(const Node &rhs) {
			const PortArg &rhs_cast = static_cast<const PortArg&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool PortArg::operator==(const PortArg &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool PortArg::operator==(const Node &rhs) const {
			const PortArg &rhs_cast = static_cast<const PortArg&>(rhs);
			return operator==(rhs_cast);
		}

		bool PortArg::operator!=(const PortArg &rhs) const {
			return !(operator==(rhs));
		}

		bool PortArg::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool PortArg::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool PortArg::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_value()) {
				if (get_value() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PortArg::replace matches multiple times (Node::value)";
					}
					set_value(new_node);
					found = true;
				}
			}
			return found;
		}

		bool PortArg::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		PortArg::ListPtr PortArg::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<PortArg>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr PortArg::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_value()) {
				list->push_back(std::static_pointer_cast<Node>(get_value()));
			}
			return list;
		}

		void PortArg::clone_children(Node::Ptr new_node) const {
			if (get_value()) {
				cast_to<PortArg>(new_node)->set_value
					(get_value()->clone());
			}
		}

		Node::Ptr PortArg::alloc_same(void) const {
			Ptr p(new PortArg);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const PortArg &p) {
			os << "PortArg: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const PortArg::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "PortArg: {nullptr}";
			}

			return os;
		}


		



	}
}