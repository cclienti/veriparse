#include <veriparse/AST/identifierscope.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

IdentifierScope::IdentifierScope(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::IdentifierScope);
	set_node_categories({NodeType::Node});
}


IdentifierScope::IdentifierScope(const IdentifierScopeLabel::ListPtr labellist, const std::string &filename, uint32_t line):
	Node(filename, line), m_labellist(labellist) {
	set_node_type(NodeType::IdentifierScope);
	set_node_categories({NodeType::Node});
}

IdentifierScope &IdentifierScope::operator=(const IdentifierScope &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &IdentifierScope::operator=(const Node &rhs) {
	const IdentifierScope &rhs_cast = static_cast<const IdentifierScope&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool IdentifierScope::operator==(const IdentifierScope &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool IdentifierScope::operator==(const Node &rhs) const {
	const IdentifierScope &rhs_cast = static_cast<const IdentifierScope&>(rhs);
	return operator==(rhs_cast);
}

bool IdentifierScope::operator!=(const IdentifierScope &rhs) const {
	return !(operator==(rhs));
}

bool IdentifierScope::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool IdentifierScope::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool IdentifierScope::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_labellist()) {
		IdentifierScopeLabel::ListPtr new_list = std::make_shared<IdentifierScopeLabel::List>();
		for (IdentifierScopeLabel::Ptr lnode : *get_labellist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "IdentifierScope::replace matches multiple times (list(IdentifierScopeLabel)::labellist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<IdentifierScopeLabel>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during IdentifierScope::replace "
								<< "of children list(IdentifierScopeLabel)::labellist";
			}
		}
		if (new_list->size() != 0) {
			set_labellist(new_list);
		}
		else {
			set_labellist(nullptr);
		}
	}
	return found;
}

bool IdentifierScope::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_labellist()) {
		IdentifierScopeLabel::ListPtr new_list = std::make_shared<IdentifierScopeLabel::List>();
		for (IdentifierScopeLabel::Ptr lnode : *get_labellist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "IdentifierScope::replace matches multiple times (list(IdentifierScopeLabel)::labellist)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<IdentifierScopeLabel>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during IdentifierScope::replace "
								<< "of children list(IdentifierScopeLabel)::labellist";
			}
		}
		if (new_list->size() != 0) {
			set_labellist(new_list);
		}
		else {
			set_labellist(nullptr);
		}
	}
	return found;
}

IdentifierScope::ListPtr IdentifierScope::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<IdentifierScope>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr IdentifierScope::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_labellist()) {
		for (const IdentifierScopeLabel::Ptr node : *get_labellist()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void IdentifierScope::clone_children(Node::Ptr new_node) const {
	cast_to<IdentifierScope>(new_node)->set_labellist
		(IdentifierScopeLabel::clone_list(get_labellist()));
}

Node::Ptr IdentifierScope::alloc_same(void) const {
	Ptr p(new IdentifierScope);
	return p;
}


std::ostream & operator<<(std::ostream &os, const IdentifierScope &p) {
	os << "IdentifierScope: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const IdentifierScope::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "IdentifierScope: {nullptr}";
	}

	return os;
}






}
}