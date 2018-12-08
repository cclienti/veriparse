#include <veriparse/AST/casezstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

CasezStatement::CasezStatement(const std::string &filename, uint32_t line):
	CaseStatement(filename, line)	{
	set_node_type(NodeType::CasezStatement);
	set_node_categories({NodeType::CaseStatement, NodeType::Node});
}


CasezStatement::CasezStatement(const Node::Ptr comp, const Case::ListPtr caselist, const std::string &filename, uint32_t line):
	CaseStatement(comp, caselist, filename, line) {
	set_node_type(NodeType::CasezStatement);
	set_node_categories({NodeType::CaseStatement, NodeType::Node});
}

CasezStatement &CasezStatement::operator=(const CasezStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &CasezStatement::operator=(const Node &rhs) {
	const CasezStatement &rhs_cast = static_cast<const CasezStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool CasezStatement::operator==(const CasezStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool CasezStatement::operator==(const Node &rhs) const {
	const CasezStatement &rhs_cast = static_cast<const CasezStatement&>(rhs);
	return operator==(rhs_cast);
}

bool CasezStatement::operator!=(const CasezStatement &rhs) const {
	return !(operator==(rhs));
}

bool CasezStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool CasezStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool CasezStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_comp()) {
		if (get_comp() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "CasezStatement::replace matches multiple times (Node::comp)";
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
										<< "CasezStatement::replace matches multiple times (list(Case)::caselist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Case>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during CasezStatement::replace "
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

bool CasezStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
										<< "CasezStatement::replace matches multiple times (list(Case)::caselist)";
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
								<< "found an empty node during CasezStatement::replace "
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

CasezStatement::ListPtr CasezStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<CasezStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr CasezStatement::get_children(void) const {
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

void CasezStatement::clone_children(Node::Ptr new_node) const {
	if (get_comp()) {
		cast_to<CasezStatement>(new_node)->set_comp
			(get_comp()->clone());
	}
	cast_to<CasezStatement>(new_node)->set_caselist
		(Case::clone_list(get_caselist()));
}

Node::Ptr CasezStatement::alloc_same(void) const {
	Ptr p(new CasezStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const CasezStatement &p) {
	os << "CasezStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const CasezStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "CasezStatement: {nullptr}";
	}

	return os;
}






}
}