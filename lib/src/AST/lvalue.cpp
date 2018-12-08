#include <veriparse/AST/lvalue.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Lvalue::Lvalue(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Lvalue);
	set_node_categories({NodeType::Node});
}


Lvalue::Lvalue(const Node::Ptr var, const std::string &filename, uint32_t line):
	Node(filename, line), m_var(var) {
	set_node_type(NodeType::Lvalue);
	set_node_categories({NodeType::Node});
}

Lvalue &Lvalue::operator=(const Lvalue &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Lvalue::operator=(const Node &rhs) {
	const Lvalue &rhs_cast = static_cast<const Lvalue&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Lvalue::operator==(const Lvalue &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Lvalue::operator==(const Node &rhs) const {
	const Lvalue &rhs_cast = static_cast<const Lvalue&>(rhs);
	return operator==(rhs_cast);
}

bool Lvalue::operator!=(const Lvalue &rhs) const {
	return !(operator==(rhs));
}

bool Lvalue::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Lvalue::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Lvalue::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_var()) {
		if (get_var() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Lvalue::replace matches multiple times (Node::var)";
			}
			set_var(new_node);
			found = true;
		}
	}
	return found;
}

bool Lvalue::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Lvalue::ListPtr Lvalue::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Lvalue>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Lvalue::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_var()) {
		list->push_back(std::static_pointer_cast<Node>(get_var()));
	}
	return list;
}

void Lvalue::clone_children(Node::Ptr new_node) const {
	if (get_var()) {
		cast_to<Lvalue>(new_node)->set_var
			(get_var()->clone());
	}
}

Node::Ptr Lvalue::alloc_same(void) const {
	Ptr p(new Lvalue);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Lvalue &p) {
	os << "Lvalue: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Lvalue::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Lvalue: {nullptr}";
	}

	return os;
}






}
}