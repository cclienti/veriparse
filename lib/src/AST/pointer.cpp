#include <veriparse/AST/pointer.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Pointer::Pointer(const std::string &filename, uint32_t line):
	Indirect(filename, line)	{
	set_node_type(NodeType::Pointer);
	set_node_categories({NodeType::Indirect, NodeType::Node});
}


Pointer::Pointer(const Node::Ptr ptr, const Node::Ptr var, const std::string &filename, uint32_t line):
	Indirect(var, filename, line), m_ptr(ptr) {
	set_node_type(NodeType::Pointer);
	set_node_categories({NodeType::Indirect, NodeType::Node});
}

Pointer &Pointer::operator=(const Pointer &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Pointer::operator=(const Node &rhs) {
	const Pointer &rhs_cast = static_cast<const Pointer&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Pointer::operator==(const Pointer &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Pointer::operator==(const Node &rhs) const {
	const Pointer &rhs_cast = static_cast<const Pointer&>(rhs);
	return operator==(rhs_cast);
}

bool Pointer::operator!=(const Pointer &rhs) const {
	return !(operator==(rhs));
}

bool Pointer::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Pointer::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Pointer::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_ptr()) {
		if (get_ptr() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Pointer::replace matches multiple times (Node::ptr)";
			}
			set_ptr(new_node);
			found = true;
		}
	}
	if (get_var()) {
		if (get_var() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Pointer::replace matches multiple times (Node::var)";
			}
			set_var(new_node);
			found = true;
		}
	}
	return found;
}

bool Pointer::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Pointer::ListPtr Pointer::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Pointer>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Pointer::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_ptr()) {
		list->push_back(std::static_pointer_cast<Node>(get_ptr()));
	}
	if (get_var()) {
		list->push_back(std::static_pointer_cast<Node>(get_var()));
	}
	return list;
}

void Pointer::clone_children(Node::Ptr new_node) const {
	if (get_ptr()) {
		cast_to<Pointer>(new_node)->set_ptr
			(get_ptr()->clone());
	}
	if (get_var()) {
		cast_to<Pointer>(new_node)->set_var
			(get_var()->clone());
	}
}

Node::Ptr Pointer::alloc_same(void) const {
	Ptr p(new Pointer);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Pointer &p) {
	os << "Pointer: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Pointer::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Pointer: {nullptr}";
	}

	return os;
}






}
}