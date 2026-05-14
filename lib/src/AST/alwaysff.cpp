// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/alwaysff.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

AlwaysFF::AlwaysFF(const std::string &filename, uint32_t line):
	Always(filename, line)	{
	set_node_type(NodeType::AlwaysFF);
	set_node_categories({NodeType::Always, NodeType::Node});
}


AlwaysFF::AlwaysFF(const Senslist::Ptr senslist, const Node::Ptr statement, const std::string &filename, uint32_t line):
	Always(senslist, statement, filename, line) {
	set_node_type(NodeType::AlwaysFF);
	set_node_categories({NodeType::Always, NodeType::Node});
}

AlwaysFF &AlwaysFF::operator=(const AlwaysFF &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &AlwaysFF::operator=(const Node &rhs) {
	const AlwaysFF &rhs_cast = static_cast<const AlwaysFF&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool AlwaysFF::operator==(const AlwaysFF &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool AlwaysFF::operator==(const Node &rhs) const {
	const AlwaysFF &rhs_cast = static_cast<const AlwaysFF&>(rhs);
	return operator==(rhs_cast);
}

bool AlwaysFF::operator!=(const AlwaysFF &rhs) const {
	return !(operator==(rhs));
}

bool AlwaysFF::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool AlwaysFF::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool AlwaysFF::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_senslist()) {
		if (get_senslist() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "AlwaysFF::replace matches multiple times (Senslist::senslist)";
			}
			set_senslist(cast_to<Senslist>(new_node));
			found = true;
		}
	}
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "AlwaysFF::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool AlwaysFF::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

AlwaysFF::ListPtr AlwaysFF::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr &p : *nodes) {
			list->push_back(cast_to<AlwaysFF>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr AlwaysFF::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_senslist()) {
		list->push_back(std::static_pointer_cast<Node>(get_senslist()));
	}
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void AlwaysFF::clone_children(Node::Ptr new_node) const {
	if (get_senslist()) {
		cast_to<AlwaysFF>(new_node)->set_senslist
			(cast_to<Senslist>(get_senslist()->clone()));
	}
	if (get_statement()) {
		cast_to<AlwaysFF>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr AlwaysFF::alloc_same(void) const {
	Ptr p(new AlwaysFF);
	return p;
}


std::ostream & operator<<(std::ostream &os, const AlwaysFF &p) {
	os << "AlwaysFF: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const AlwaysFF::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "AlwaysFF: {nullptr}";
	}

	return os;
}






}
}