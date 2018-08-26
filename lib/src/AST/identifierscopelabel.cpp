#include <veriparse/AST/identifierscopelabel.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		IdentifierScopeLabel::IdentifierScopeLabel(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::IdentifierScopeLabel);
			set_node_categories({NodeType::Node});
		}

		
		IdentifierScopeLabel::IdentifierScopeLabel(const Node::Ptr loop, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_loop(loop), m_name(name) {
			set_node_type(NodeType::IdentifierScopeLabel);
			set_node_categories({NodeType::Node});
		}
		
		IdentifierScopeLabel &IdentifierScopeLabel::operator=(const IdentifierScopeLabel &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &IdentifierScopeLabel::operator=(const Node &rhs) {
			const IdentifierScopeLabel &rhs_cast = static_cast<const IdentifierScopeLabel&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool IdentifierScopeLabel::operator==(const IdentifierScopeLabel &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool IdentifierScopeLabel::operator==(const Node &rhs) const {
			const IdentifierScopeLabel &rhs_cast = static_cast<const IdentifierScopeLabel&>(rhs);
			return operator==(rhs_cast);
		}

		bool IdentifierScopeLabel::operator!=(const IdentifierScopeLabel &rhs) const {
			return !(operator==(rhs));
		}

		bool IdentifierScopeLabel::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool IdentifierScopeLabel::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool IdentifierScopeLabel::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_loop()) {
				if (get_loop() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "IdentifierScopeLabel::replace matches multiple times (Node::loop)";
					}
					set_loop(new_node);
					found = true;
				}
			}
			return found;
		}

		bool IdentifierScopeLabel::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		IdentifierScopeLabel::ListPtr IdentifierScopeLabel::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<IdentifierScopeLabel>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr IdentifierScopeLabel::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_loop()) {
				list->push_back(std::static_pointer_cast<Node>(get_loop()));
			}
			return list;
		}

		void IdentifierScopeLabel::clone_children(Node::Ptr new_node) const {
			if (get_loop()) {
				cast_to<IdentifierScopeLabel>(new_node)->set_loop
					(get_loop()->clone());
			}
		}

		Node::Ptr IdentifierScopeLabel::alloc_same(void) const {
			Ptr p(new IdentifierScopeLabel);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const IdentifierScopeLabel &p) {
			os << "IdentifierScopeLabel: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const IdentifierScopeLabel::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "IdentifierScopeLabel: {nullptr}";
			}

			return os;
		}


		



	}
}