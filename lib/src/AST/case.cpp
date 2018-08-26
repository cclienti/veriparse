#include <veriparse/AST/case.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Case::Case(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Case);
			set_node_categories({NodeType::Node});
		}

		
		Case::Case(const Node::ListPtr cond, const Node::Ptr statement, const std::string &filename, uint32_t line):
			Node(filename, line), m_cond(cond), m_statement(statement) {
			set_node_type(NodeType::Case);
			set_node_categories({NodeType::Node});
		}
		
		Case &Case::operator=(const Case &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Case::operator=(const Node &rhs) {
			const Case &rhs_cast = static_cast<const Case&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Case::operator==(const Case &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Case::operator==(const Node &rhs) const {
			const Case &rhs_cast = static_cast<const Case&>(rhs);
			return operator==(rhs_cast);
		}

		bool Case::operator!=(const Case &rhs) const {
			return !(operator==(rhs));
		}

		bool Case::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Case::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Case::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_cond()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_cond()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Case::replace matches multiple times (list(Node)::cond)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Case::replace "
						            << "of children list(Node)::cond";
					}
				}
				if (new_list->size() != 0) {
					set_cond(new_list);
				}
				else {
					set_cond(nullptr);
				}
			}
			if (get_statement()) {
				if (get_statement() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Case::replace matches multiple times (Node::statement)";
					}
					set_statement(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Case::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_cond()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_cond()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Case::replace matches multiple times (list(Node)::cond)";
							}
							if(new_nodes) {
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(n);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Case::replace "
						            << "of children list(Node)::cond";
					}
				}
				if (new_list->size() != 0) {
					set_cond(new_list);
				}
				else {
					set_cond(nullptr);
				}
			}
			return found;
		}

		Case::ListPtr Case::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Case>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Case::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_cond()) {
				for (const Node::Ptr node : *get_cond()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			if (get_statement()) {
				list->push_back(std::static_pointer_cast<Node>(get_statement()));
			}
			return list;
		}

		void Case::clone_children(Node::Ptr new_node) const {
			cast_to<Case>(new_node)->set_cond
				(Node::clone_list(get_cond()));
			if (get_statement()) {
				cast_to<Case>(new_node)->set_statement
					(get_statement()->clone());
			}
		}

		Node::Ptr Case::alloc_same(void) const {
			Ptr p(new Case);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Case &p) {
			os << "Case: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Case::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Case: {nullptr}";
			}

			return os;
		}


		



	}
}