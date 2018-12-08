#include <veriparse/AST/intconst.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

IntConst::IntConst(const std::string &filename, uint32_t line):
	Constant(filename, line)	{
	set_node_type(NodeType::IntConst);
	set_node_categories({NodeType::Constant, NodeType::Node});
}


IntConst::IntConst(const std::string &value, const std::string &filename, uint32_t line):
	Constant(filename, line), m_value(value) {
	set_node_type(NodeType::IntConst);
	set_node_categories({NodeType::Constant, NodeType::Node});
}

IntConst &IntConst::operator=(const IntConst &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_value(rhs.get_value());
	return *this;
}

Node &IntConst::operator=(const Node &rhs) {
	const IntConst &rhs_cast = static_cast<const IntConst&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool IntConst::operator==(const IntConst &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_value() != rhs.get_value()) {
		return false;
	}
	return true;
}

bool IntConst::operator==(const Node &rhs) const {
	const IntConst &rhs_cast = static_cast<const IntConst&>(rhs);
	return operator==(rhs_cast);
}

bool IntConst::operator!=(const IntConst &rhs) const {
	return !(operator==(rhs));
}

bool IntConst::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool IntConst::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool IntConst::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	return found;
}

bool IntConst::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

IntConst::ListPtr IntConst::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<IntConst>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr IntConst::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	return list;
}

void IntConst::clone_children(Node::Ptr new_node) const {
}

Node::Ptr IntConst::alloc_same(void) const {
	Ptr p(new IntConst);
	return p;
}


std::ostream & operator<<(std::ostream &os, const IntConst &p) {
	os << "IntConst: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "value: " << p.get_value();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const IntConst::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "IntConst: {nullptr}";
	}

	return os;
}






}
}