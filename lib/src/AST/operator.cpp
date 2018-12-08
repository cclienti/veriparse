#include <veriparse/AST/operator.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Operator::Operator(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Operator);
	set_node_categories({NodeType::Node});
}


Operator::Operator(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Node(filename, line), m_left(left), m_right(right) {
	set_node_type(NodeType::Operator);
	set_node_categories({NodeType::Node});
}

Operator &Operator::operator=(const Operator &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Operator::operator=(const Node &rhs) {
	const Operator &rhs_cast = static_cast<const Operator&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Operator::operator==(const Operator &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Operator::operator==(const Node &rhs) const {
	const Operator &rhs_cast = static_cast<const Operator&>(rhs);
	return operator==(rhs_cast);
}

bool Operator::operator!=(const Operator &rhs) const {
	return !(operator==(rhs));
}

bool Operator::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Operator::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Operator::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Operator::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Operator::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Operator::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Operator::ListPtr Operator::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Operator>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Operator::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Operator::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<Operator>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<Operator>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Operator::alloc_same(void) const {
	Ptr p(new Operator);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Operator &p) {
	os << "Operator: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Operator::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Operator: {nullptr}";
	}

	return os;
}






}
}