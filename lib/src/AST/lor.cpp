#include <veriparse/AST/lor.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Lor::Lor(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::Lor);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


Lor::Lor(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::Lor);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

Lor &Lor::operator=(const Lor &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Lor::operator=(const Node &rhs) {
	const Lor &rhs_cast = static_cast<const Lor&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Lor::operator==(const Lor &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Lor::operator==(const Node &rhs) const {
	const Lor &rhs_cast = static_cast<const Lor&>(rhs);
	return operator==(rhs_cast);
}

bool Lor::operator!=(const Lor &rhs) const {
	return !(operator==(rhs));
}

bool Lor::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Lor::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Lor::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Lor::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Lor::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Lor::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Lor::ListPtr Lor::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Lor>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Lor::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Lor::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<Lor>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<Lor>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Lor::alloc_same(void) const {
	Ptr p(new Lor);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Lor &p) {
	os << "Lor: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Lor::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Lor: {nullptr}";
	}

	return os;
}






}
}