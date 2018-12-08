#include <veriparse/AST/uor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Uor::Uor(const std::string &filename, uint32_t line):
	UnaryOperator(filename, line)	{
	set_node_type(NodeType::Uor);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}


Uor::Uor(const Node::Ptr right, const std::string &filename, uint32_t line):
	UnaryOperator(right, filename, line) {
	set_node_type(NodeType::Uor);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}

Uor &Uor::operator=(const Uor &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Uor::operator=(const Node &rhs) {
	const Uor &rhs_cast = static_cast<const Uor&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Uor::operator==(const Uor &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Uor::operator==(const Node &rhs) const {
	const Uor &rhs_cast = static_cast<const Uor&>(rhs);
	return operator==(rhs_cast);
}

bool Uor::operator!=(const Uor &rhs) const {
	return !(operator==(rhs));
}

bool Uor::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Uor::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Uor::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Uor::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Uor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Uor::ListPtr Uor::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Uor>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Uor::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Uor::clone_children(Node::Ptr new_node) const {
	if (get_right()) {
		cast_to<Uor>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Uor::alloc_same(void) const {
	Ptr p(new Uor);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Uor &p) {
	os << "Uor: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Uor::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Uor: {nullptr}";
	}

	return os;
}






}
}