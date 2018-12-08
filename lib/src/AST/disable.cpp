#include <veriparse/AST/disable.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Disable::Disable(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Disable);
	set_node_categories({NodeType::Node});
}


Disable::Disable(const std::string &dest, const std::string &filename, uint32_t line):
	Node(filename, line), m_dest(dest) {
	set_node_type(NodeType::Disable);
	set_node_categories({NodeType::Node});
}

Disable &Disable::operator=(const Disable &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_dest(rhs.get_dest());
	return *this;
}

Node &Disable::operator=(const Node &rhs) {
	const Disable &rhs_cast = static_cast<const Disable&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Disable::operator==(const Disable &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_dest() != rhs.get_dest()) {
		return false;
	}
	return true;
}

bool Disable::operator==(const Node &rhs) const {
	const Disable &rhs_cast = static_cast<const Disable&>(rhs);
	return operator==(rhs_cast);
}

bool Disable::operator!=(const Disable &rhs) const {
	return !(operator==(rhs));
}

bool Disable::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Disable::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Disable::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	return found;
}

bool Disable::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Disable::ListPtr Disable::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Disable>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Disable::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	return list;
}

void Disable::clone_children(Node::Ptr new_node) const {
}

Node::Ptr Disable::alloc_same(void) const {
	Ptr p(new Disable);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Disable &p) {
	os << "Disable: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "dest: " << p.get_dest();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Disable::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Disable: {nullptr}";
	}

	return os;
}






}
}