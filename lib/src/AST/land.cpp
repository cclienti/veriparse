#include <veriparse/AST/land.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Land::Land(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::Land);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


Land::Land(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::Land);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

Land &Land::operator=(const Land &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Land::operator=(const Node &rhs) {
	const Land &rhs_cast = static_cast<const Land&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Land::operator==(const Land &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Land::operator==(const Node &rhs) const {
	const Land &rhs_cast = static_cast<const Land&>(rhs);
	return operator==(rhs_cast);
}

bool Land::operator!=(const Land &rhs) const {
	return !(operator==(rhs));
}

bool Land::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Land::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Land::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Land::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Land::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool Land::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Land::ListPtr Land::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Land>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Land::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Land::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<Land>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<Land>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr Land::alloc_same(void) const {
	Ptr p(new Land);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Land &p) {
	os << "Land: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Land::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Land: {nullptr}";
	}

	return os;
}






}
}