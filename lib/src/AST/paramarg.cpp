#include <veriparse/AST/paramarg.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		ParamArg::ParamArg(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::ParamArg);
			set_node_categories({NodeType::Node});
		}

		
		ParamArg::ParamArg(const Node::Ptr value, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_value(value), m_name(name) {
			set_node_type(NodeType::ParamArg);
			set_node_categories({NodeType::Node});
		}
		
		ParamArg &ParamArg::operator=(const ParamArg &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &ParamArg::operator=(const Node &rhs) {
			const ParamArg &rhs_cast = static_cast<const ParamArg&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool ParamArg::operator==(const ParamArg &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool ParamArg::operator==(const Node &rhs) const {
			const ParamArg &rhs_cast = static_cast<const ParamArg&>(rhs);
			return operator==(rhs_cast);
		}

		bool ParamArg::operator!=(const ParamArg &rhs) const {
			return !(operator==(rhs));
		}

		bool ParamArg::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool ParamArg::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool ParamArg::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_value()) {
				if (get_value() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "ParamArg::replace matches multiple times (Node::value)";
					}
					set_value(new_node);
					found = true;
				}
			}
			return found;
		}

		bool ParamArg::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		ParamArg::ListPtr ParamArg::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<ParamArg>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr ParamArg::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_value()) {
				list->push_back(std::static_pointer_cast<Node>(get_value()));
			}
			return list;
		}

		void ParamArg::clone_children(Node::Ptr new_node) const {
			if (get_value()) {
				cast_to<ParamArg>(new_node)->set_value
					(get_value()->clone());
			}
		}

		Node::Ptr ParamArg::alloc_same(void) const {
			Ptr p(new ParamArg);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const ParamArg &p) {
			os << "ParamArg: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const ParamArg::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "ParamArg: {nullptr}";
			}

			return os;
		}


		



	}
}