#include <veriparse/AST/eventstatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

EventStatement::EventStatement(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::EventStatement);
	set_node_categories({NodeType::Node});
}


EventStatement::EventStatement(const Senslist::Ptr senslist, const Node::Ptr statement, const std::string &filename, uint32_t line):
	Node(filename, line), m_senslist(senslist), m_statement(statement) {
	set_node_type(NodeType::EventStatement);
	set_node_categories({NodeType::Node});
}

EventStatement &EventStatement::operator=(const EventStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &EventStatement::operator=(const Node &rhs) {
	const EventStatement &rhs_cast = static_cast<const EventStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool EventStatement::operator==(const EventStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool EventStatement::operator==(const Node &rhs) const {
	const EventStatement &rhs_cast = static_cast<const EventStatement&>(rhs);
	return operator==(rhs_cast);
}

bool EventStatement::operator!=(const EventStatement &rhs) const {
	return !(operator==(rhs));
}

bool EventStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool EventStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool EventStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_senslist()) {
		if (get_senslist() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "EventStatement::replace matches multiple times (Senslist::senslist)";
			}
			set_senslist(cast_to<Senslist>(new_node));
			found = true;
		}
	}
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "EventStatement::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool EventStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

EventStatement::ListPtr EventStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<EventStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr EventStatement::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_senslist()) {
		list->push_back(std::static_pointer_cast<Node>(get_senslist()));
	}
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void EventStatement::clone_children(Node::Ptr new_node) const {
	if (get_senslist()) {
		cast_to<EventStatement>(new_node)->set_senslist
			(cast_to<Senslist>(get_senslist()->clone()));
	}
	if (get_statement()) {
		cast_to<EventStatement>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr EventStatement::alloc_same(void) const {
	Ptr p(new EventStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const EventStatement &p) {
	os << "EventStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const EventStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "EventStatement: {nullptr}";
	}

	return os;
}






}
}