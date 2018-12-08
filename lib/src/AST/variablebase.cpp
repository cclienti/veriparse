#include <veriparse/AST/variablebase.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

VariableBase::VariableBase(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::VariableBase);
	set_node_categories({NodeType::Node});
}


VariableBase::VariableBase(const std::string &name, const std::string &filename, uint32_t line):
	Node(filename, line), m_name(name) {
	set_node_type(NodeType::VariableBase);
	set_node_categories({NodeType::Node});
}

VariableBase &VariableBase::operator=(const VariableBase &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	return *this;
}

Node &VariableBase::operator=(const Node &rhs) {
	const VariableBase &rhs_cast = static_cast<const VariableBase&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool VariableBase::operator==(const VariableBase &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	return true;
}

bool VariableBase::operator==(const Node &rhs) const {
	const VariableBase &rhs_cast = static_cast<const VariableBase&>(rhs);
	return operator==(rhs_cast);
}

bool VariableBase::operator!=(const VariableBase &rhs) const {
	return !(operator==(rhs));
}

bool VariableBase::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool VariableBase::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool VariableBase::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	return found;
}

bool VariableBase::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

VariableBase::ListPtr VariableBase::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<VariableBase>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr VariableBase::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	return list;
}

void VariableBase::clone_children(Node::Ptr new_node) const {
}

Node::Ptr VariableBase::alloc_same(void) const {
	Ptr p(new VariableBase);
	return p;
}


std::ostream & operator<<(std::ostream &os, const VariableBase &p) {
	os << "VariableBase: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const VariableBase::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "VariableBase: {nullptr}";
	}

	return os;
}






}
}