#include <veriparse/AST/casestatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		CaseStatement::CaseStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::CaseStatement);
			set_node_categories({NodeType::Node});
		}

		
		CaseStatement::CaseStatement(const Node::Ptr comp, const Case::ListPtr caselist, const std::string &filename, uint32_t line):
			Node(filename, line), m_comp(comp), m_caselist(caselist) {
			set_node_type(NodeType::CaseStatement);
			set_node_categories({NodeType::Node});
		}
		
		CaseStatement &CaseStatement::operator=(const CaseStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &CaseStatement::operator=(const Node &rhs) {
			const CaseStatement &rhs_cast = static_cast<const CaseStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool CaseStatement::operator==(const CaseStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool CaseStatement::operator==(const Node &rhs) const {
			const CaseStatement &rhs_cast = static_cast<const CaseStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool CaseStatement::operator!=(const CaseStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool CaseStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool CaseStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool CaseStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_comp()) {
				if (get_comp() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "CaseStatement::replace matches multiple times (Node::comp)";
					}
					set_comp(new_node);
					found = true;
				}
			}
			if (get_caselist()) {
				Case::ListPtr new_list = std::make_shared<Case::List>();
				for (Case::Ptr lnode : *get_caselist()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "CaseStatement::replace matches multiple times (list(Case)::caselist)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Case>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during CaseStatement::replace "
						            << "of children list(Case)::caselist";
					}
				}
				if (new_list->size() != 0) {
					set_caselist(new_list);
				}
				else {
					set_caselist(nullptr);
				}
			}
			return found;
		}

		bool CaseStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_caselist()) {
				Case::ListPtr new_list = std::make_shared<Case::List>();
				for (Case::Ptr lnode : *get_caselist()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "CaseStatement::replace matches multiple times (list(Case)::caselist)";
							}
							if(new_nodes) {
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(cast_to<Case>(n));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during CaseStatement::replace "
						            << "of children list(Case)::caselist";
					}
				}
				if (new_list->size() != 0) {
					set_caselist(new_list);
				}
				else {
					set_caselist(nullptr);
				}
			}
			return found;
		}

		CaseStatement::ListPtr CaseStatement::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<CaseStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr CaseStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_comp()) {
				list->push_back(std::static_pointer_cast<Node>(get_comp()));
			}
			if (get_caselist()) {
				for (const Case::Ptr node : *get_caselist()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void CaseStatement::clone_children(Node::Ptr new_node) const {
			if (get_comp()) {
				cast_to<CaseStatement>(new_node)->set_comp
					(get_comp()->clone());
			}
			cast_to<CaseStatement>(new_node)->set_caselist
				(Case::clone_list(get_caselist()));
		}

		Node::Ptr CaseStatement::alloc_same(void) const {
			Ptr p(new CaseStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const CaseStatement &p) {
			os << "CaseStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const CaseStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "CaseStatement: {nullptr}";
			}

			return os;
		}


		



	}
}