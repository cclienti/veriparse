#include <veriparse/AST/delaystatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

DelayStatement::DelayStatement(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::DelayStatement);
	set_node_categories({NodeType::Node});
}


DelayStatement::DelayStatement(const Node::Ptr delay, const std::string &filename, uint32_t line):
	Node(filename, line), m_delay(delay) {
	set_node_type(NodeType::DelayStatement);
	set_node_categories({NodeType::Node});
}

DelayStatement &DelayStatement::operator=(const DelayStatement &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &DelayStatement::operator=(const Node &rhs) {
	const DelayStatement &rhs_cast = static_cast<const DelayStatement&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool DelayStatement::operator==(const DelayStatement &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool DelayStatement::operator==(const Node &rhs) const {
	const DelayStatement &rhs_cast = static_cast<const DelayStatement&>(rhs);
	return operator==(rhs_cast);
}

bool DelayStatement::operator!=(const DelayStatement &rhs) const {
	return !(operator==(rhs));
}

bool DelayStatement::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool DelayStatement::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool DelayStatement::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_delay()) {
		if (get_delay() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "DelayStatement::replace matches multiple times (Node::delay)";
			}
			set_delay(new_node);
			found = true;
		}
	}
	return found;
}

bool DelayStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

DelayStatement::ListPtr DelayStatement::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<DelayStatement>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr DelayStatement::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_delay()) {
		list->push_back(std::static_pointer_cast<Node>(get_delay()));
	}
	return list;
}

void DelayStatement::clone_children(Node::Ptr new_node) const {
	if (get_delay()) {
		cast_to<DelayStatement>(new_node)->set_delay
			(get_delay()->clone());
	}
}

Node::Ptr DelayStatement::alloc_same(void) const {
	Ptr p(new DelayStatement);
	return p;
}


std::ostream & operator<<(std::ostream &os, const DelayStatement &p) {
	os << "DelayStatement: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const DelayStatement::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "DelayStatement: {nullptr}";
	}

	return os;
}






}
}