#include <veriparse/AST/source.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Source::Source(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Source);
			set_node_categories({NodeType::Node});
		}

		
		Source::Source(const Description::Ptr description, const std::string &filename, uint32_t line):
			Node(filename, line), m_description(description) {
			set_node_type(NodeType::Source);
			set_node_categories({NodeType::Node});
		}
		
		Source &Source::operator=(const Source &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Source::operator=(const Node &rhs) {
			const Source &rhs_cast = static_cast<const Source&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Source::operator==(const Source &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Source::operator==(const Node &rhs) const {
			const Source &rhs_cast = static_cast<const Source&>(rhs);
			return operator==(rhs_cast);
		}

		bool Source::operator!=(const Source &rhs) const {
			return !(operator==(rhs));
		}

		bool Source::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Source::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Source::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_description()) {
				if (get_description() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Source::replace matches multiple times (Description::description)";
					}
					set_description(cast_to<Description>(new_node));
					found = true;
				}
			}
			return found;
		}

		bool Source::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Source::ListPtr Source::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Source>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Source::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_description()) {
				list->push_back(std::static_pointer_cast<Node>(get_description()));
			}
			return list;
		}

		void Source::clone_children(Node::Ptr new_node) const {
			if (get_description()) {
				cast_to<Source>(new_node)->set_description
					(cast_to<Description>(get_description()->clone()));
			}
		}

		Node::Ptr Source::alloc_same(void) const {
			Ptr p(new Source);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Source &p) {
			os << "Source: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Source::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Source: {nullptr}";
			}

			return os;
		}


		



	}
}