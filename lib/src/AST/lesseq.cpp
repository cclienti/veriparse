#include <veriparse/AST/lesseq.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

LessEq::LessEq(const std::string &filename, uint32_t line):
	Operator(filename, line)	{
	set_node_type(NodeType::LessEq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}


LessEq::LessEq(const Node::Ptr left, const Node::Ptr right, const std::string &filename, uint32_t line):
	Operator(left, right, filename, line) {
	set_node_type(NodeType::LessEq);
	set_node_categories({NodeType::Operator, NodeType::Node});
}

LessEq &LessEq::operator=(const LessEq &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &LessEq::operator=(const Node &rhs) {
	const LessEq &rhs_cast = static_cast<const LessEq&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool LessEq::operator==(const LessEq &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool LessEq::operator==(const Node &rhs) const {
	const LessEq &rhs_cast = static_cast<const LessEq&>(rhs);
	return operator==(rhs_cast);
}

bool LessEq::operator!=(const LessEq &rhs) const {
	return !(operator==(rhs));
}

bool LessEq::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool LessEq::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool LessEq::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "LessEq::replace matches multiple times (Node::left)";
			}
			set_left(new_node);
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "LessEq::replace matches multiple times (Node::right)";
			}
			set_right(new_node);
			found = true;
		}
	}
	return found;
}

bool LessEq::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

LessEq::ListPtr LessEq::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<LessEq>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr LessEq::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void LessEq::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<LessEq>(new_node)->set_left
			(get_left()->clone());
	}
	if (get_right()) {
		cast_to<LessEq>(new_node)->set_right
			(get_right()->clone());
	}
}

Node::Ptr LessEq::alloc_same(void) const {
	Ptr p(new LessEq);
	return p;
}


std::ostream & operator<<(std::ostream &os, const LessEq &p) {
	os << "LessEq: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const LessEq::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "LessEq: {nullptr}";
	}

	return os;
}






}
}