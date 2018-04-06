#include <veriparse/AST/waitstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		WaitStatement::WaitStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::WaitStatement);
			set_node_categories({NodeType::Node});
		}

		
		WaitStatement::WaitStatement(const Node::Ptr cond, const Node::Ptr statement, const std::string &filename, uint32_t line):
			Node(filename, line), m_cond(cond), m_statement(statement) {
			set_node_type(NodeType::WaitStatement);
			set_node_categories({NodeType::Node});
		}
		
		WaitStatement &WaitStatement::operator=(const WaitStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &WaitStatement::operator=(const Node &rhs) {
			const WaitStatement &rhs_cast = static_cast<const WaitStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool WaitStatement::operator==(const WaitStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool WaitStatement::operator==(const Node &rhs) const {
			const WaitStatement &rhs_cast = static_cast<const WaitStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool WaitStatement::operator!=(const WaitStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool WaitStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool WaitStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool WaitStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_cond()) {
				if (get_cond() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "WaitStatement::replace matches multiple times (Node::cond)";
					}
					set_cond(new_node);
					found = true;
				}
			}
			if (get_statement()) {
				if (get_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "WaitStatement::replace matches multiple times (Node::statement)";
					}
					set_statement(new_node);
					found = true;
				}
			}
			return found;
		}

		bool WaitStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		WaitStatement::ListPtr WaitStatement::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<WaitStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr WaitStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_cond()) {
				list->push_back(std::static_pointer_cast<Node>(get_cond()));
			}
			if (get_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_statement()));
			}
			return list;
		}

		void WaitStatement::clone_children(Node::Ptr new_node) const {
			if (get_cond()) {
				cast_to<WaitStatement>(new_node)->set_cond
					(get_cond()->clone());
			}
			if (get_statement()) {
				cast_to<WaitStatement>(new_node)->set_statement
					(get_statement()->clone());
			}
		}

		Node::Ptr WaitStatement::alloc_same(void) const {
			Ptr p(new WaitStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const WaitStatement &p) {
			os << "WaitStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const WaitStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "WaitStatement: {nullptr}";
			}

			return os;
		}


		



	}
}