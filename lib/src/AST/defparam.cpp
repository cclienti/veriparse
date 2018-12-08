#include <veriparse/AST/defparam.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Defparam::Defparam(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Defparam);
	set_node_categories({NodeType::Node});
}


Defparam::Defparam(const Identifier::Ptr identifier, const Rvalue::Ptr right, const std::string &filename, uint32_t line):
	Node(filename, line), m_identifier(identifier), m_right(right) {
	set_node_type(NodeType::Defparam);
	set_node_categories({NodeType::Node});
}

Defparam &Defparam::operator=(const Defparam &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Defparam::operator=(const Node &rhs) {
	const Defparam &rhs_cast = static_cast<const Defparam&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Defparam::operator==(const Defparam &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Defparam::operator==(const Node &rhs) const {
	const Defparam &rhs_cast = static_cast<const Defparam&>(rhs);
	return operator==(rhs_cast);
}

bool Defparam::operator!=(const Defparam &rhs) const {
	return !(operator==(rhs));
}

bool Defparam::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Defparam::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Defparam::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_identifier()) {
		if (get_identifier() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Defparam::replace matches multiple times (Identifier::identifier)";
			}
			set_identifier(cast_to<Identifier>(new_node));
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Defparam::replace matches multiple times (Rvalue::right)";
			}
			set_right(cast_to<Rvalue>(new_node));
			found = true;
		}
	}
	return found;
}

bool Defparam::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Defparam::ListPtr Defparam::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Defparam>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Defparam::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_identifier()) {
		list->push_back(std::static_pointer_cast<Node>(get_identifier()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Defparam::clone_children(Node::Ptr new_node) const {
	if (get_identifier()) {
		cast_to<Defparam>(new_node)->set_identifier
			(cast_to<Identifier>(get_identifier()->clone()));
	}
	if (get_right()) {
		cast_to<Defparam>(new_node)->set_right
			(cast_to<Rvalue>(get_right()->clone()));
	}
}

Node::Ptr Defparam::alloc_same(void) const {
	Ptr p(new Defparam);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Defparam &p) {
	os << "Defparam: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Defparam::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Defparam: {nullptr}";
	}

	return os;
}






}
}