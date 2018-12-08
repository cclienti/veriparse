#include <veriparse/AST/defparamlist.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Defparamlist::Defparamlist(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Defparamlist);
	set_node_categories({NodeType::Node});
}


Defparamlist::Defparamlist(const Defparam::ListPtr list, const std::string &filename, uint32_t line):
	Node(filename, line), m_list(list) {
	set_node_type(NodeType::Defparamlist);
	set_node_categories({NodeType::Node});
}

Defparamlist &Defparamlist::operator=(const Defparamlist &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Defparamlist::operator=(const Node &rhs) {
	const Defparamlist &rhs_cast = static_cast<const Defparamlist&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Defparamlist::operator==(const Defparamlist &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Defparamlist::operator==(const Node &rhs) const {
	const Defparamlist &rhs_cast = static_cast<const Defparamlist&>(rhs);
	return operator==(rhs_cast);
}

bool Defparamlist::operator!=(const Defparamlist &rhs) const {
	return !(operator==(rhs));
}

bool Defparamlist::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Defparamlist::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Defparamlist::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_list()) {
		Defparam::ListPtr new_list = std::make_shared<Defparam::List>();
		for (Defparam::Ptr lnode : *get_list()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Defparamlist::replace matches multiple times (list(Defparam)::list)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Defparam>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Defparamlist::replace "
								<< "of children list(Defparam)::list";
			}
		}
		if (new_list->size() != 0) {
			set_list(new_list);
		}
		else {
			set_list(nullptr);
		}
	}
	return found;
}

bool Defparamlist::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_list()) {
		Defparam::ListPtr new_list = std::make_shared<Defparam::List>();
		for (Defparam::Ptr lnode : *get_list()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Defparamlist::replace matches multiple times (list(Defparam)::list)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<Defparam>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Defparamlist::replace "
								<< "of children list(Defparam)::list";
			}
		}
		if (new_list->size() != 0) {
			set_list(new_list);
		}
		else {
			set_list(nullptr);
		}
	}
	return found;
}

Defparamlist::ListPtr Defparamlist::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Defparamlist>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Defparamlist::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_list()) {
		for (const Defparam::Ptr node : *get_list()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void Defparamlist::clone_children(Node::Ptr new_node) const {
	cast_to<Defparamlist>(new_node)->set_list
		(Defparam::clone_list(get_list()));
}

Node::Ptr Defparamlist::alloc_same(void) const {
	Ptr p(new Defparamlist);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Defparamlist &p) {
	os << "Defparamlist: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Defparamlist::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Defparamlist: {nullptr}";
	}

	return os;
}






}
}