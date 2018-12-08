#include <veriparse/AST/partselectminusindexed.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

PartselectMinusIndexed::PartselectMinusIndexed(const std::string &filename, uint32_t line):
	PartselectIndexed(filename, line)	{
	set_node_type(NodeType::PartselectMinusIndexed);
	set_node_categories({NodeType::PartselectIndexed, NodeType::Indirect, NodeType::Node});
}


PartselectMinusIndexed::PartselectMinusIndexed(const Node::Ptr index, const Node::Ptr size, const Node::Ptr var, const std::string &filename, uint32_t line):
	PartselectIndexed(index, size, var, filename, line) {
	set_node_type(NodeType::PartselectMinusIndexed);
	set_node_categories({NodeType::PartselectIndexed, NodeType::Indirect, NodeType::Node});
}

PartselectMinusIndexed &PartselectMinusIndexed::operator=(const PartselectMinusIndexed &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &PartselectMinusIndexed::operator=(const Node &rhs) {
	const PartselectMinusIndexed &rhs_cast = static_cast<const PartselectMinusIndexed&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool PartselectMinusIndexed::operator==(const PartselectMinusIndexed &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool PartselectMinusIndexed::operator==(const Node &rhs) const {
	const PartselectMinusIndexed &rhs_cast = static_cast<const PartselectMinusIndexed&>(rhs);
	return operator==(rhs_cast);
}

bool PartselectMinusIndexed::operator!=(const PartselectMinusIndexed &rhs) const {
	return !(operator==(rhs));
}

bool PartselectMinusIndexed::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool PartselectMinusIndexed::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool PartselectMinusIndexed::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_index()) {
		if (get_index() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "PartselectMinusIndexed::replace matches multiple times (Node::index)";
			}
			set_index(new_node);
			found = true;
		}
	}
	if (get_size()) {
		if (get_size() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "PartselectMinusIndexed::replace matches multiple times (Node::size)";
			}
			set_size(new_node);
			found = true;
		}
	}
	if (get_var()) {
		if (get_var() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "PartselectMinusIndexed::replace matches multiple times (Node::var)";
			}
			set_var(new_node);
			found = true;
		}
	}
	return found;
}

bool PartselectMinusIndexed::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	return found;
}

PartselectMinusIndexed::ListPtr PartselectMinusIndexed::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<PartselectMinusIndexed>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr PartselectMinusIndexed::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_index()) {
		list->push_back(std::static_pointer_cast<Node>(get_index()));
	}
	if (get_size()) {
		list->push_back(std::static_pointer_cast<Node>(get_size()));
	}
	if (get_var()) {
		list->push_back(std::static_pointer_cast<Node>(get_var()));
	}
	return list;
}

void PartselectMinusIndexed::clone_children(Node::Ptr new_node) const {
	if (get_index()) {
		cast_to<PartselectMinusIndexed>(new_node)->set_index
			(get_index()->clone());
	}
	if (get_size()) {
		cast_to<PartselectMinusIndexed>(new_node)->set_size
			(get_size()->clone());
	}
	if (get_var()) {
		cast_to<PartselectMinusIndexed>(new_node)->set_var
			(get_var()->clone());
	}
}

Node::Ptr PartselectMinusIndexed::alloc_same(void) const {
	Ptr p(new PartselectMinusIndexed);
	return p;
}


std::ostream & operator<<(std::ostream &os, const PartselectMinusIndexed &p) {
	os << "PartselectMinusIndexed: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const PartselectMinusIndexed::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "PartselectMinusIndexed: {nullptr}";
	}

	return os;
}






}
}