#include <veriparse/AST/uand.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Uand::Uand(const std::string &filename, uint32_t line):
	UnaryOperator(filename, line)	{
	set_node_type(NodeType::Uand);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}


Uand::Uand(const Node::Ptr right, const std::string &filename, uint32_t line):
	UnaryOperator(right, filename, line) {
	set_node_type(NodeType::Uand);
	set_node_categories({NodeType::UnaryOperator, NodeType::Node});
}

Uand &Uand::operator=(const Uand &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Uand::operator=(const Node &rhs) {
	const Uand &rhs_cast = static_cast<const Uand&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Uand::operator==(const Uand &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Uand::operator==(const Node &rhs) const {
	const Uand &rhs_cast = static_cast<const Uand&>(rhs);
	return operator==(rhs_cast);
}

bool Uand::operator!=(const Uand &rhs) const {
	return !(operator==(rhs));
}

bool Uand::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Uand::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Uand::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Uand::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Uand::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Uand::ListPtr Uand::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Uand>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Uand::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Uand::clone_children(Node::Ptr new_node) const {
	if (get_right()) {
		cast_to<Uand>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Uand::alloc_same(void) const {
	Ptr p(new Uand);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Uand &p) {
	os << "Uand: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Uand::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Uand: {nullptr}";
	}

	return os;
}






}
}