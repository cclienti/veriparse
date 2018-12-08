#include <veriparse/AST/eq.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Eq::Eq(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::Eq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


Eq::Eq(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::Eq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

Eq &Eq::operator=(const Eq &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Eq::operator=(const Node &rhs) {
	const Eq &rhs_cast = static_cast<const Eq&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Eq::operator==(const Eq &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Eq::operator==(const Node &rhs) const {
	const Eq &rhs_cast = static_cast<const Eq&>(rhs);
	return operator==(rhs_cast);
}

bool Eq::operator!=(const Eq &rhs) const {
	return !(operator==(rhs));
}

bool Eq::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Eq::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Eq::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Eq::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Eq::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Eq::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Eq::ListPtr Eq::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Eq>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Eq::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Eq::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<Eq>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<Eq>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Eq::alloc_same(void) const {
	Ptr p(new Eq);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Eq &p) {
	os << "Eq: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Eq::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Eq: {nullptr}";
	}

	return os;
}






}
}