#include <veriparse/AST/width.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Width::Width(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Width);
	set_node_categories({NodeType::Node});
}


Width::Width(const Node::Ptr msb, const Node::Ptr lsb, const std::string &filename, uint32_t line):
	Node(filename, line), m_msb(msb), m_lsb(lsb) {
	set_node_type(NodeType::Width);
	set_node_categories({NodeType::Node});
}

Width &Width::operator=(const Width &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Width::operator=(const Node &rhs) {
	const Width &rhs_cast = static_cast<const Width&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Width::operator==(const Width &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Width::operator==(const Node &rhs) const {
	const Width &rhs_cast = static_cast<const Width&>(rhs);
	return operator==(rhs_cast);
}

bool Width::operator!=(const Width &rhs) const {
	return !(operator==(rhs));
}

bool Width::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Width::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Width::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_msb()) {
		if (get_msb() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Width::replace matches multiple times (Node::msb)";
			}
			set_msb(new_node);
			found = true;
		}
	}
	if (get_lsb()) {
		if (get_lsb() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Width::replace matches multiple times (Node::lsb)";
			}
			set_lsb(new_node);
			found = true;
		}
	}
	return found;
}

bool Width::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

Width::ListPtr Width::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Width>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Width::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_msb()) {
		list->push_back(std::static_pointer_cast<Node>(get_msb()));
	}
	if (get_lsb()) {
		list->push_back(std::static_pointer_cast<Node>(get_lsb()));
	}
	return list;
}

void Width::clone_children(Node::Ptr new_node) const {
	if (get_msb()) {
		cast_to<Width>(new_node)->set_msb
			(get_msb()->clone());
	}
	if (get_lsb()) {
		cast_to<Width>(new_node)->set_lsb
			(get_lsb()->clone());
	}
}

Node::Ptr Width::alloc_same(void) const {
	Ptr p(new Width);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Width &p) {
	os << "Width: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Width::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Width: {nullptr}";
	}

	return os;
}






}
}