#include <veriparse/AST/foreverstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

ForeverStatement::ForeverStatement(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::ForeverStatement);
	set_node_categories({NodeType::Node});
}


ForeverStatement::ForeverStatement(const Node::Ptr statement, const std::string &filename, uint32_t line):
	Node(filename, line), m_statement(statement) {
	set_node_type(NodeType::ForeverStatement);
	set_node_categories({NodeType::Node});
}

ForeverStatement &ForeverStatement::operator=(const ForeverStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &ForeverStatement::operator=(const Node &rhs) {
	const ForeverStatement &rhs_cast = static_cast<const ForeverStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool ForeverStatement::operator==(const ForeverStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool ForeverStatement::operator==(const Node &rhs) const {
	const ForeverStatement &rhs_cast = static_cast<const ForeverStatement&>(rhs);
	return operator==(rhs_cast);
}

bool ForeverStatement::operator!=(const ForeverStatement &rhs) const {
	return !(operator==(rhs));
}

bool ForeverStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool ForeverStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool ForeverStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "ForeverStatement::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool ForeverStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

ForeverStatement::ListPtr ForeverStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<ForeverStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr ForeverStatement::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void ForeverStatement::clone_children(Node::Ptr new_node) const {
	if (get_statement()) {
		cast_to<ForeverStatement>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr ForeverStatement::alloc_same(void) const {
	Ptr p(new ForeverStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const ForeverStatement &p) {
	os << "ForeverStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const ForeverStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "ForeverStatement: {nullptr}";
	}

	return os;
}






}
}