#include <veriparse/AST/initial.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Initial::Initial(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Initial);
	set_node_categories({NodeType::Node});
}


Initial::Initial(const Node::Ptr statement, const std::string &filename, uint32_t line):
	Node(filename, line), m_statement(statement) {
	set_node_type(NodeType::Initial);
	set_node_categories({NodeType::Node});
}

Initial &Initial::operator=(const Initial &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Initial::operator=(const Node &rhs) {
	const Initial &rhs_cast = static_cast<const Initial&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Initial::operator==(const Initial &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Initial::operator==(const Node &rhs) const {
	const Initial &rhs_cast = static_cast<const Initial&>(rhs);
	return operator==(rhs_cast);
}

bool Initial::operator!=(const Initial &rhs) const {
	return !(operator==(rhs));
}

bool Initial::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Initial::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Initial::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Initial::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool Initial::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Initial::ListPtr Initial::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Initial>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Initial::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void Initial::clone_children(Node::Ptr new_node) const {
	if (get_statement()) {
		cast_to<Initial>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr Initial::alloc_same(void) const {
	Ptr p(new Initial);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Initial &p) {
	os << "Initial: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Initial::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Initial: {nullptr}";
	}

	return os;
}






}
}