#include <veriparse/AST/sll.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Sll::Sll(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::Sll);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


Sll::Sll(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::Sll);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

Sll &Sll::operator=(const Sll &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Sll::operator=(const Node &rhs) {
	const Sll &rhs_cast = static_cast<const Sll&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Sll::operator==(const Sll &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Sll::operator==(const Node &rhs) const {
	const Sll &rhs_cast = static_cast<const Sll&>(rhs);
	return operator==(rhs_cast);
}

bool Sll::operator!=(const Sll &rhs) const {
	return !(operator==(rhs));
}

bool Sll::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Sll::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Sll::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Sll::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Sll::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Sll::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Sll::ListPtr Sll::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Sll>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Sll::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Sll::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<Sll>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<Sll>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Sll::alloc_same(void) const {
	Ptr p(new Sll);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Sll &p) {
	os << "Sll: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Sll::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Sll: {nullptr}";
	}

	return os;
}






}
}