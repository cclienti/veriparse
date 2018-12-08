#include <veriparse/AST/identifier.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Identifier::Identifier(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Identifier);
	set_node_categories({NodeType::Node});
}


Identifier::Identifier(const IdentifierScope::Ptr scope, const std::string &name, const std::string &filename, uint32_t line):
	Node(filename, line), m_scope(scope), m_name(name) {
	set_node_type(NodeType::Identifier);
	set_node_categories({NodeType::Node});
}

Identifier &Identifier::operator=(const Identifier &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	return *this;
}

Node &Identifier::operator=(const Node &rhs) {
	const Identifier &rhs_cast = static_cast<const Identifier&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Identifier::operator==(const Identifier &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	return true;
}

bool Identifier::operator==(const Node &rhs) const {
	const Identifier &rhs_cast = static_cast<const Identifier&>(rhs);
	return operator==(rhs_cast);
}

bool Identifier::operator!=(const Identifier &rhs) const {
	return !(operator==(rhs));
}

bool Identifier::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Identifier::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Identifier::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_scope()) {
		if (get_scope() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Identifier::replace matches multiple times (IdentifierScope::scope)";
			}
			set_scope(cast_to<IdentifierScope>(new_node));
			found = true;
		}
	}
	return found;
}

bool Identifier::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Identifier::ListPtr Identifier::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Identifier>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Identifier::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_scope()) {
		list->push_back(std::static_pointer_cast<Node>(get_scope()));
	}
	return list;
}

void Identifier::clone_children(Node::Ptr new_node) const {
	if (get_scope()) {
		cast_to<Identifier>(new_node)->set_scope
			(cast_to<IdentifierScope>(get_scope()->clone()));
	}
}

Node::Ptr Identifier::alloc_same(void) const {
	Ptr p(new Identifier);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Identifier &p) {
	os << "Identifier: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Identifier::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Identifier: {nullptr}";
	}

	return os;
}






}
}