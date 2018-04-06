#include <veriparse/AST/ifstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		IfStatement::IfStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::IfStatement);
			set_node_categories({NodeType::Node});
		}

		
		IfStatement::IfStatement(const Node::Ptr cond, const Node::Ptr true_statement, const Node::Ptr false_statement, const std::string &filename, uint32_t line):
			Node(filename, line), m_cond(cond), m_true_statement(true_statement), m_false_statement(false_statement) {
			set_node_type(NodeType::IfStatement);
			set_node_categories({NodeType::Node});
		}
		
		IfStatement &IfStatement::operator=(const IfStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &IfStatement::operator=(const Node &rhs) {
			const IfStatement &rhs_cast = static_cast<const IfStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool IfStatement::operator==(const IfStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool IfStatement::operator==(const Node &rhs) const {
			const IfStatement &rhs_cast = static_cast<const IfStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool IfStatement::operator!=(const IfStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool IfStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool IfStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool IfStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_cond()) {
				if (get_cond() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "IfStatement::replace matches multiple times (Node::cond)";
					}
					set_cond(new_node);
					found = true;
				}
			}
			if (get_true_statement()) {
				if (get_true_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "IfStatement::replace matches multiple times (Node::true_statement)";
					}
					set_true_statement(new_node);
					found = true;
				}
			}
			if (get_false_statement()) {
				if (get_false_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "IfStatement::replace matches multiple times (Node::false_statement)";
					}
					set_false_statement(new_node);
					found = true;
				}
			}
			return found;
		}

		bool IfStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		IfStatement::ListPtr IfStatement::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<IfStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr IfStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_cond()) {
				list->push_back(std::static_pointer_cast<Node>(get_cond()));
			}
			if (get_true_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_true_statement()));
			}
			if (get_false_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_false_statement()));
			}
			return list;
		}

		void IfStatement::clone_children(Node::Ptr new_node) const {
			if (get_cond()) {
				cast_to<IfStatement>(new_node)->set_cond
					(get_cond()->clone());
			}
			if (get_true_statement()) {
				cast_to<IfStatement>(new_node)->set_true_statement
					(get_true_statement()->clone());
			}
			if (get_false_statement()) {
				cast_to<IfStatement>(new_node)->set_false_statement
					(get_false_statement()->clone());
			}
		}

		Node::Ptr IfStatement::alloc_same(void) const {
			Ptr p(new IfStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const IfStatement &p) {
			os << "IfStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const IfStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "IfStatement: {nullptr}";
			}

			return os;
		}


		



	}
}