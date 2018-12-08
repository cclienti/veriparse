#include <veriparse/AST/uplus.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Uplus::Uplus(const std::string &filename, uint32_t line):
	UnaryOperator(filename, line)	{
	set_node_type(NodeType::Uplus);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}


Uplus::Uplus(const Node::Ptr right, const std::string &filename, uint32_t line):
	UnaryOperator(right, filename, line) {
	set_node_type(NodeType::Uplus);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}

Uplus &Uplus::operator=(const Uplus &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Uplus::operator=(const Node &rhs) {
	const Uplus &rhs_cast = static_cast<const Uplus&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Uplus::operator==(const Uplus &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Uplus::operator==(const Node &rhs) const {
	const Uplus &rhs_cast = static_cast<const Uplus&>(rhs);
	return operator==(rhs_cast);
}

bool Uplus::operator!=(const Uplus &rhs) const {
	return !(operator==(rhs));
}

bool Uplus::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Uplus::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Uplus::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Uplus::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Uplus::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Uplus::ListPtr Uplus::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Uplus>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Uplus::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Uplus::clone_children(Node::Ptr new_node) const {
	if (get_right()) {
		cast_to<Uplus>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Uplus::alloc_same(void) const {
	Ptr p(new Uplus);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Uplus &p) {
	os << "Uplus: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Uplus::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Uplus: {nullptr}";
	}

	return os;
}






}
}