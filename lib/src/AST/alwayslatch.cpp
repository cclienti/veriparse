// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/AST/alwayslatch.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

AlwaysLatch::AlwaysLatch(const std::string &filename, uint32_t line):
	Always(filename, line)	{
	set_node_type(NodeType::AlwaysLatch);
	set_node_categories({NodeType::Always, NodeType::Node});
}


AlwaysLatch::AlwaysLatch(const Senslist::Ptr senslist, const Node::Ptr statement, const std::string &filename, uint32_t line):
	Always(senslist, statement, filename, line) {
	set_node_type(NodeType::AlwaysLatch);
	set_node_categories({NodeType::Always, NodeType::Node});
}

AlwaysLatch &AlwaysLatch::operator=(const AlwaysLatch &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &AlwaysLatch::operator=(const Node &rhs) {
	const AlwaysLatch &rhs_cast = static_cast<const AlwaysLatch&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool AlwaysLatch::operator==(const AlwaysLatch &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool AlwaysLatch::operator==(const Node &rhs) const {
	const AlwaysLatch &rhs_cast = static_cast<const AlwaysLatch&>(rhs);
	return operator==(rhs_cast);
}

bool AlwaysLatch::operator!=(const AlwaysLatch &rhs) const {
	return !(operator==(rhs));
}

bool AlwaysLatch::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool AlwaysLatch::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool AlwaysLatch::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_senslist()) {
		if (get_senslist() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "AlwaysLatch::replace matches multiple times (Senslist::senslist)";
			}
			set_senslist(cast_to<Senslist>(new_node));
			found = true;
		}
	}
	if (get_statement()) {
		if (get_statement() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "AlwaysLatch::replace matches multiple times (Node::statement)";
			}
			set_statement(new_node);
			found = true;
		}
	}
	return found;
}

bool AlwaysLatch::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

AlwaysLatch::ListPtr AlwaysLatch::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr &p : *nodes) {
			list->push_back(cast_to<AlwaysLatch>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr AlwaysLatch::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_senslist()) {
		list->push_back(std::static_pointer_cast<Node>(get_senslist()));
	}
	if (get_statement()) {
		list->push_back(std::static_pointer_cast<Node>(get_statement()));
	}
	return list;
}

void AlwaysLatch::clone_children(Node::Ptr new_node) const {
	if (get_senslist()) {
		cast_to<AlwaysLatch>(new_node)->set_senslist
			(cast_to<Senslist>(get_senslist()->clone()));
	}
	if (get_statement()) {
		cast_to<AlwaysLatch>(new_node)->set_statement
			(get_statement()->clone());
	}
}

Node::Ptr AlwaysLatch::alloc_same(void) const {
	Ptr p(new AlwaysLatch);
	return p;
}


std::ostream & operator<<(std::ostream &os, const AlwaysLatch &p) {
	os << "AlwaysLatch: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const AlwaysLatch::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "AlwaysLatch: {nullptr}";
	}

	return os;
}






}
}