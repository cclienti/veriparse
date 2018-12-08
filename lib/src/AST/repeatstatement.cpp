#include <veriparse/AST/repeatstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

RepeatStatement::RepeatStatement(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::RepeatStatement);
	set_node_categories({NodeType::Node});
}


RepeatStatement::RepeatStatement(const Node::Ptr times, const Node::Ptr statement, const std::string &filename, uint32_t line):
	Node(filename, line), m_times(times), m_statement(statement) {
	set_node_type(NodeType::RepeatStatement);
	set_node_categories({NodeType::Node});
}

RepeatStatement &RepeatStatement::operator=(const RepeatStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &RepeatStatement::operator=(const Node &rhs) {
	const RepeatStatement &rhs_cast = static_cast<const RepeatStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool RepeatStatement::operator==(const RepeatStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool RepeatStatement::operator==(const Node &rhs) const {
	const RepeatStatement &rhs_cast = static_cast<const RepeatStatement&>(rhs);
	return operator==(rhs_cast);
}

bool RepeatStatement::operator!=(const RepeatStatement &rhs) const {
	return !(operator==(rhs));
}

bool RepeatStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool RepeatStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool RepeatStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_times()) {
		if (get_times() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "RepeatStatement::replace matches multiple times (Node::times)";
			}
			set_times(new_node);
			found = true;
		}
	}
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "RepeatStatement::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool RepeatStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

RepeatStatement::ListPtr RepeatStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<RepeatStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr RepeatStatement::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_times()) {
		list->push_back(std::static_pointer_cast<Node>(get_times()));
	}
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void RepeatStatement::clone_children(Node::Ptr new_node) const {
	if (get_times()) {
		cast_to<RepeatStatement>(new_node)->set_times
			(get_times()->clone());
	}
	if (get_statement()) {
		cast_to<RepeatStatement>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr RepeatStatement::alloc_same(void) const {
	Ptr p(new RepeatStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const RepeatStatement &p) {
	os << "RepeatStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const RepeatStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "RepeatStatement: {nullptr}";
	}

	return os;
}






}
}