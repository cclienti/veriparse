#include <veriparse/AST/casexstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

CasexStatement::CasexStatement(const std::string &filename, uint32_t line):
	CaseStatement(filename, line)	{
	set_node_type(NodeType::CasexStatement);
	set_node_categories({NodeType::CaseStatement, NodeType::Node});
}


CasexStatement::CasexStatement(const Node::Ptr comp, const Case::ListPtr caselist, const std::string &filename, uint32_t line):
	CaseStatement(comp, caselist, filename, line) {
	set_node_type(NodeType::CasexStatement);
	set_node_categories({NodeType::CaseStatement, NodeType::Node});
}

CasexStatement &CasexStatement::operator=(const CasexStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &CasexStatement::operator=(const Node &rhs) {
	const CasexStatement &rhs_cast = static_cast<const CasexStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool CasexStatement::operator==(const CasexStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool CasexStatement::operator==(const Node &rhs) const {
	const CasexStatement &rhs_cast = static_cast<const CasexStatement&>(rhs);
	return operator==(rhs_cast);
}

bool CasexStatement::operator!=(const CasexStatement &rhs) const {
	return !(operator==(rhs));
}

bool CasexStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool CasexStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool CasexStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_comp()) {
		if (get_comp() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "CasexStatement::replace matches multiple times (Node::comp)";
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
										<< "CasexStatement::replace matches multiple times (list(Case)::caselist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Case>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during CasexStatement::replace "
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

bool CasexStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
										<< "CasexStatement::replace matches multiple times (list(Case)::caselist)";
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
								<< "found an empty node during CasexStatement::replace "
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

CasexStatement::ListPtr CasexStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<CasexStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr CasexStatement::get_children(void) const {
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

void CasexStatement::clone_children(Node::Ptr new_node) const {
	if (get_comp()) {
		cast_to<CasexStatement>(new_node)->set_comp
			(get_comp()->clone());
	}
	cast_to<CasexStatement>(new_node)->set_caselist
		(Case::clone_list(get_caselist()));
}

Node::Ptr CasexStatement::alloc_same(void) const {
	Ptr p(new CasexStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const CasexStatement &p) {
	os << "CasexStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const CasexStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "CasexStatement: {nullptr}";
	}

	return os;
}






}
}