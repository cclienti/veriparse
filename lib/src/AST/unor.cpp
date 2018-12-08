#include <veriparse/AST/unor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Unor::Unor(const std::string &filename, uint32_t line):
	UnaryOperator(filename, line)	{
	set_node_type(NodeType::Unor);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}


Unor::Unor(const Node::Ptr right, const std::string &filename, uint32_t line):
	UnaryOperator(right, filename, line) {
	set_node_type(NodeType::Unor);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}

Unor &Unor::operator=(const Unor &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Unor::operator=(const Node &rhs) {
	const Unor &rhs_cast = static_cast<const Unor&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Unor::operator==(const Unor &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Unor::operator==(const Node &rhs) const {
	const Unor &rhs_cast = static_cast<const Unor&>(rhs);
	return operator==(rhs_cast);
}

bool Unor::operator!=(const Unor &rhs) const {
	return !(operator==(rhs));
}

bool Unor::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Unor::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Unor::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Unor::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Unor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Unor::ListPtr Unor::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Unor>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Unor::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Unor::clone_children(Node::Ptr new_node) const {
	if (get_right()) {
		cast_to<Unor>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Unor::alloc_same(void) const {
	Ptr p(new Unor);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Unor &p) {
	os << "Unor: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Unor::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Unor: {nullptr}";
	}

	return os;
}






}
}