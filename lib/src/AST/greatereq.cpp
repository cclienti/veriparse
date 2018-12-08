#include <veriparse/AST/greatereq.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

GreaterEq::GreaterEq(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::GreaterEq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


GreaterEq::GreaterEq(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::GreaterEq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

GreaterEq &GreaterEq::operator=(const GreaterEq &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &GreaterEq::operator=(const Node &rhs) {
	const GreaterEq &rhs_cast = static_cast<const GreaterEq&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool GreaterEq::operator==(const GreaterEq &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool GreaterEq::operator==(const Node &rhs) const {
	const GreaterEq &rhs_cast = static_cast<const GreaterEq&>(rhs);
	return operator==(rhs_cast);
}

bool GreaterEq::operator!=(const GreaterEq &rhs) const {
	return !(operator==(rhs));
}

bool GreaterEq::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool GreaterEq::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool GreaterEq::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "GreaterEq::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "GreaterEq::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool GreaterEq::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

GreaterEq::ListPtr GreaterEq::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<GreaterEq>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr GreaterEq::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void GreaterEq::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<GreaterEq>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<GreaterEq>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr GreaterEq::alloc_same(void) const {
	Ptr p(new GreaterEq);
	return p;
}


std::ostream & operator<<(std::ostream &os, const GreaterEq &p) {
	os << "GreaterEq: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const GreaterEq::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "GreaterEq: {nullptr}";
	}

	return os;
}






}
}