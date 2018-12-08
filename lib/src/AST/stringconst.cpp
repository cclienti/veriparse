#include <veriparse/AST/stringconst.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

StringConst::StringConst(const std::string &filename, uint32_t line):
	Constant(filename, line)	{
	set_node_type(NodeType::StringConst);
	set_node_categories({NodeType::Constant, NodeType::Node});
}


StringConst::StringConst(const std::string &value, const std::string &filename, uint32_t line):
	Constant(filename, line), m_value(value) {
	set_node_type(NodeType::StringConst);
	set_node_categories({NodeType::Constant, NodeType::Node});
}

StringConst &StringConst::operator=(const StringConst &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_value(rhs.get_value());
	return *this;
}

Node &StringConst::operator=(const Node &rhs) {
	const StringConst &rhs_cast = static_cast<const StringConst&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool StringConst::operator==(const StringConst &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_value() != rhs.get_value()) {
		return false;
	}
	return true;
}

bool StringConst::operator==(const Node &rhs) const {
	const StringConst &rhs_cast = static_cast<const StringConst&>(rhs);
	return operator==(rhs_cast);
}

bool StringConst::operator!=(const StringConst &rhs) const {
	return !(operator==(rhs));
}

bool StringConst::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool StringConst::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool StringConst::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	return found;
}

bool StringConst::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

StringConst::ListPtr StringConst::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<StringConst>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr StringConst::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	return list;
}

void StringConst::clone_children(Node::Ptr new_node) const {
}

Node::Ptr StringConst::alloc_same(void) const {
	Ptr p(new StringConst);
	return p;
}


std::ostream & operator<<(std::ostream &os, const StringConst &p) {
	os << "StringConst: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "value: " << p.get_value();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const StringConst::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "StringConst: {nullptr}";
	}

	return os;
}






}
}