#include <veriparse/AST/whilestatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		WhileStatement::WhileStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::WhileStatement);
			set_node_categories({NodeType::Node});
		}

		
		WhileStatement::WhileStatement(const Node::Ptr cond, const Node::Ptr statement, const std::string &filename, uint32_t line):
			Node(filename, line), m_cond(cond), m_statement(statement) {
			set_node_type(NodeType::WhileStatement);
			set_node_categories({NodeType::Node});
		}
		
		WhileStatement &WhileStatement::operator=(const WhileStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &WhileStatement::operator=(const Node &rhs) {
			const WhileStatement &rhs_cast = static_cast<const WhileStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool WhileStatement::operator==(const WhileStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool WhileStatement::operator==(const Node &rhs) const {
			const WhileStatement &rhs_cast = static_cast<const WhileStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool WhileStatement::operator!=(const WhileStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool WhileStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool WhileStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool WhileStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_cond()) {
				if (get_cond() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "WhileStatement::replace matches multiple times (Node::cond)";
					}
					set_cond(new_node);
					found = true;
				}
			}
			if (get_statement()) {
				if (get_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "WhileStatement::replace matches multiple times (Node::statement)";
					}
					set_statement(new_node);
					found = true;
				}
			}
			return found;
		}

		bool WhileStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		WhileStatement::ListPtr WhileStatement::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<WhileStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr WhileStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_cond()) {
				list->push_back(std::static_pointer_cast<Node>(get_cond()));
			}
			if (get_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_statement()));
			}
			return list;
		}

		void WhileStatement::clone_children(Node::Ptr new_node) const {
			if (get_cond()) {
				cast_to<WhileStatement>(new_node)->set_cond
					(get_cond()->clone());
			}
			if (get_statement()) {
				cast_to<WhileStatement>(new_node)->set_statement
					(get_statement()->clone());
			}
		}

		Node::Ptr WhileStatement::alloc_same(void) const {
			Ptr p(new WhileStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const WhileStatement &p) {
			os << "WhileStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const WhileStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "WhileStatement: {nullptr}";
			}

			return os;
		}


		



	}
}