#include <veriparse/AST/nonblockingsubstitution.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

NonblockingSubstitution::NonblockingSubstitution(const std::string &filename, uint32_t line):
	Assign(filename, line)	{
	set_node_type(NodeType::NonblockingSubstitution);
	set_node_categories({NodeType::Assign, NodeType::Node});
}


NonblockingSubstitution::NonblockingSubstitution(const Lvalue::Ptr left, const Rvalue::Ptr right, const DelayStatement::Ptr ldelay, const DelayStatement::Ptr rdelay, const std::string &filename, uint32_t line):
	Assign(left, right, ldelay, rdelay, filename, line) {
	set_node_type(NodeType::NonblockingSubstitution);
	set_node_categories({NodeType::Assign, NodeType::Node});
}

NonblockingSubstitution &NonblockingSubstitution::operator=(const NonblockingSubstitution &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &NonblockingSubstitution::operator=(const Node &rhs) {
	const NonblockingSubstitution &rhs_cast = static_cast<const NonblockingSubstitution&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool NonblockingSubstitution::operator==(const NonblockingSubstitution &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool NonblockingSubstitution::operator==(const Node &rhs) const {
	const NonblockingSubstitution &rhs_cast = static_cast<const NonblockingSubstitution&>(rhs);
	return operator==(rhs_cast);
}

bool NonblockingSubstitution::operator!=(const NonblockingSubstitution &rhs) const {
	return !(operator==(rhs));
}

bool NonblockingSubstitution::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool NonblockingSubstitution::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool NonblockingSubstitution::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_left()) {
		if (get_left() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "NonblockingSubstitution::replace matches multiple times (Lvalue::left)";
			}
			set_left(cast_to<Lvalue>(new_node));
			found = true;
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "NonblockingSubstitution::replace matches multiple times (Rvalue::right)";
			}
			set_right(cast_to<Rvalue>(new_node));
			found = true;
		}
	}
	if (get_ldelay()) {
		if (get_ldelay() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "NonblockingSubstitution::replace matches multiple times (DelayStatement::ldelay)";
			}
			set_ldelay(cast_to<DelayStatement>(new_node));
			found = true;
		}
	}
	if (get_rdelay()) {
		if (get_rdelay() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "NonblockingSubstitution::replace matches multiple times (DelayStatement::rdelay)";
			}
			set_rdelay(cast_to<DelayStatement>(new_node));
			found = true;
		}
	}
	return found;
}

bool NonblockingSubstitution::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

NonblockingSubstitution::ListPtr NonblockingSubstitution::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<NonblockingSubstitution>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr NonblockingSubstitution::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_left()) {
		list->push_back(std::static_pointer_cast<Node>(get_left()));
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	if (get_ldelay()) {
		list->push_back(std::static_pointer_cast<Node>(get_ldelay()));
	}
	if (get_rdelay()) {
		list->push_back(std::static_pointer_cast<Node>(get_rdelay()));
	}
	return list;
}

void NonblockingSubstitution::clone_children(Node::Ptr new_node) const {
	if (get_left()) {
		cast_to<NonblockingSubstitution>(new_node)->set_left
			(cast_to<Lvalue>(get_left()->clone()));
	}
	if (get_right()) {
		cast_to<NonblockingSubstitution>(new_node)->set_right
			(cast_to<Rvalue>(get_right()->clone()));
	}
	if (get_ldelay()) {
		cast_to<NonblockingSubstitution>(new_node)->set_ldelay
			(cast_to<DelayStatement>(get_ldelay()->clone()));
	}
	if (get_rdelay()) {
		cast_to<NonblockingSubstitution>(new_node)->set_rdelay
			(cast_to<DelayStatement>(get_rdelay()->clone()));
	}
}

Node::Ptr NonblockingSubstitution::alloc_same(void) const {
	Ptr p(new NonblockingSubstitution);
	return p;
}


std::ostream & operator<<(std::ostream &os, const NonblockingSubstitution &p) {
	os << "NonblockingSubstitution: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const NonblockingSubstitution::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "NonblockingSubstitution: {nullptr}";
	}

	return os;
}






}
}