#include <veriparse/AST/forstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		ForStatement::ForStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::ForStatement);
			set_node_categories({NodeType::Node});
		}

		
		ForStatement::ForStatement(const BlockingSubstitution::Ptr pre, const Node::Ptr cond, const BlockingSubstitution::Ptr post, const Node::Ptr statement, const std::string &filename, uint32_t line):
			Node(filename, line), m_pre(pre), m_cond(cond), m_post(post), m_statement(statement) {
			set_node_type(NodeType::ForStatement);
			set_node_categories({NodeType::Node});
		}
		
		ForStatement &ForStatement::operator=(const ForStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &ForStatement::operator=(const Node &rhs) {
			const ForStatement &rhs_cast = static_cast<const ForStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool ForStatement::operator==(const ForStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool ForStatement::operator==(const Node &rhs) const {
			const ForStatement &rhs_cast = static_cast<const ForStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool ForStatement::operator!=(const ForStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool ForStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool ForStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool ForStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_pre()) {
				if (get_pre() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "ForStatement::replace matches multiple times (BlockingSubstitution::pre)";
					}
					set_pre(cast_to<BlockingSubstitution>(new_node));
					found = true;
				}
			}
			if (get_cond()) {
				if (get_cond() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "ForStatement::replace matches multiple times (Node::cond)";
					}
					set_cond(new_node);
					found = true;
				}
			}
			if (get_post()) {
				if (get_post() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "ForStatement::replace matches multiple times (BlockingSubstitution::post)";
					}
					set_post(cast_to<BlockingSubstitution>(new_node));
					found = true;
				}
			}
			if (get_statement()) {
				if (get_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "ForStatement::replace matches multiple times (Node::statement)";
					}
					set_statement(new_node);
					found = true;
				}
			}
			return found;
		}

		bool ForStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		ForStatement::ListPtr ForStatement::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<ForStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr ForStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_pre()) {
				list->push_back(std::static_pointer_cast<Node>(get_pre()));
			}
			if (get_cond()) {
				list->push_back(std::static_pointer_cast<Node>(get_cond()));
			}
			if (get_post()) {
				list->push_back(std::static_pointer_cast<Node>(get_post()));
			}
			if (get_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_statement()));
			}
			return list;
		}

		void ForStatement::clone_children(Node::Ptr new_node) const {
			if (get_pre()) {
				cast_to<ForStatement>(new_node)->set_pre
					(cast_to<BlockingSubstitution>(get_pre()->clone()));
			}
			if (get_cond()) {
				cast_to<ForStatement>(new_node)->set_cond
					(get_cond()->clone());
			}
			if (get_post()) {
				cast_to<ForStatement>(new_node)->set_post
					(cast_to<BlockingSubstitution>(get_post()->clone()));
			}
			if (get_statement()) {
				cast_to<ForStatement>(new_node)->set_statement
					(get_statement()->clone());
			}
		}

		Node::Ptr ForStatement::alloc_same(void) const {
			Ptr p(new ForStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const ForStatement &p) {
			os << "ForStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const ForStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "ForStatement: {nullptr}";
			}

			return os;
		}


		



	}
}