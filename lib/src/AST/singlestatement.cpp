#include <veriparse/AST/singlestatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

SingleStatement::SingleStatement(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::SingleStatement);
	set_node_categories({NodeType::Node});
}


SingleStatement::SingleStatement(const Node::Ptr statement, const DelayStatement::Ptr delay, const std::string &scope, const std::string &filename, uint32_t line):
	Node(filename, line), m_statement(statement), m_delay(delay), m_scope(scope) {
	set_node_type(NodeType::SingleStatement);
	set_node_categories({NodeType::Node});
}

SingleStatement &SingleStatement::operator=(const SingleStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_scope(rhs.get_scope());
	return *this;
}

Node &SingleStatement::operator=(const Node &rhs) {
	const SingleStatement &rhs_cast = static_cast<const SingleStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool SingleStatement::operator==(const SingleStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_scope() != rhs.get_scope()) {
		return false;
	}
	return true;
}

bool SingleStatement::operator==(const Node &rhs) const {
	const SingleStatement &rhs_cast = static_cast<const SingleStatement&>(rhs);
	return operator==(rhs_cast);
}

bool SingleStatement::operator!=(const SingleStatement &rhs) const {
	return !(operator==(rhs));
}

bool SingleStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool SingleStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool SingleStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "SingleStatement::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	if (get_delay()) {
		if (get_delay() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "SingleStatement::replace matches multiple times (DelayStatement::delay)";
			}
			set_delay(cast_to<DelayStatement>(new_node));
			found = true;
		}
	}
	return found;
}

bool SingleStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

SingleStatement::ListPtr SingleStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<SingleStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr SingleStatement::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	if (get_delay()) {
		list->push_back(std::static_pointer_cast<Node>(get_delay()));
	}
	return list;
}

void SingleStatement::clone_children(Node::Ptr new_node) const {
	if (get_statement()) {
		cast_to<SingleStatement>(new_node)->set_statement
			(get_statement()->clone());
	}
	if (get_delay()) {
		cast_to<SingleStatement>(new_node)->set_delay
			(cast_to<DelayStatement>(get_delay()->clone()));
	}
}

Node::Ptr SingleStatement::alloc_same(void) const {
	Ptr p(new SingleStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const SingleStatement &p) {
	os << "SingleStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "scope: " << p.get_scope();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const SingleStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "SingleStatement: {nullptr}";
	}

	return os;
}






}
}