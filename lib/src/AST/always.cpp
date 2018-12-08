#include <veriparse/AST/always.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Always::Always(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Always);
	set_node_categories({NodeType::Node});
}


Always::Always(const Senslist::Ptr senslist, const Node::Ptr statement, const std::string &filename, uint32_t line):
	Node(filename, line), m_senslist(senslist), m_statement(statement) {
	set_node_type(NodeType::Always);
	set_node_categories({NodeType::Node});
}

Always &Always::operator=(const Always &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Always::operator=(const Node &rhs) {
	const Always &rhs_cast = static_cast<const Always&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Always::operator==(const Always &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Always::operator==(const Node &rhs) const {
	const Always &rhs_cast = static_cast<const Always&>(rhs);
	return operator==(rhs_cast);
}

bool Always::operator!=(const Always &rhs) const {
	return !(operator==(rhs));
}

bool Always::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Always::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Always::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_senslist()) {
		if (get_senslist() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Always::replace matches multiple times (Senslist::senslist)";
			}
			set_senslist(cast_to<Senslist>(new_node));
			found = true;
		}
	}
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Always::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool Always::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Always::ListPtr Always::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Always>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Always::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_senslist()) {
		list->push_back(std::static_pointer_cast<Node>(get_senslist()));
	}
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void Always::clone_children(Node::Ptr new_node) const {
	if (get_senslist()) {
		cast_to<Always>(new_node)->set_senslist
			(cast_to<Senslist>(get_senslist()->clone()));
	}
	if (get_statement()) {
		cast_to<Always>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr Always::alloc_same(void) const {
	Ptr p(new Always);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Always &p) {
	os << "Always: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Always::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Always: {nullptr}";
	}

	return os;
}






}
}