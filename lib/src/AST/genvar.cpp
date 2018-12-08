#include <veriparse/AST/genvar.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Genvar::Genvar(const std::string &filename, uint32_t line):
	VariableBase(filename, line)	{
	set_node_type(NodeType::Genvar);
	set_node_categories({NodeType::VariableBase, NodeType::Node});
}


Genvar::Genvar(const std::string &name, const std::string &filename, uint32_t line):
	VariableBase(name, filename, line) {
	set_node_type(NodeType::Genvar);
	set_node_categories({NodeType::VariableBase, NodeType::Node});
}

Genvar &Genvar::operator=(const Genvar &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	return *this;
}

Node &Genvar::operator=(const Node &rhs) {
	const Genvar &rhs_cast = static_cast<const Genvar&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Genvar::operator==(const Genvar &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	return true;
}

bool Genvar::operator==(const Node &rhs) const {
	const Genvar &rhs_cast = static_cast<const Genvar&>(rhs);
	return operator==(rhs_cast);
}

bool Genvar::operator!=(const Genvar &rhs) const {
	return !(operator==(rhs));
}

bool Genvar::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Genvar::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Genvar::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	return found;
}

bool Genvar::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Genvar::ListPtr Genvar::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Genvar>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Genvar::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	return list;
}

void Genvar::clone_children(Node::Ptr new_node) const {
}

Node::Ptr Genvar::alloc_same(void) const {
	Ptr p(new Genvar);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Genvar &p) {
	os << "Genvar: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Genvar::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Genvar: {nullptr}";
	}

	return os;
}






}
}