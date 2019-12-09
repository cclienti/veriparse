#include <veriparse/AST/pragmalist.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Pragmalist::Pragmalist(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Pragmalist);
	set_node_categories({NodeType::Node});
}


Pragmalist::Pragmalist(const Pragma::ListPtr pragmas, const Node::ListPtr statements, const std::string &filename, uint32_t line):
	Node(filename, line), m_pragmas(pragmas), m_statements(statements) {
	set_node_type(NodeType::Pragmalist);
	set_node_categories({NodeType::Node});
}

Pragmalist &Pragmalist::operator=(const Pragmalist &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	return *this;
}

Node &Pragmalist::operator=(const Node &rhs) {
	const Pragmalist &rhs_cast = static_cast<const Pragmalist&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Pragmalist::operator==(const Pragmalist &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	return true;
}

bool Pragmalist::operator==(const Node &rhs) const {
	const Pragmalist &rhs_cast = static_cast<const Pragmalist&>(rhs);
	return operator==(rhs_cast);
}

bool Pragmalist::operator!=(const Pragmalist &rhs) const {
	return !(operator==(rhs));
}

bool Pragmalist::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Pragmalist::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Pragmalist::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_pragmas()) {
		Pragma::ListPtr new_list = std::make_shared<Pragma::List>();
		for (Pragma::Ptr lnode : *get_pragmas()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Pragmalist::replace matches multiple times (list(Pragma)::pragmas)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Pragma>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Pragmalist::replace "
								<< "of children list(Pragma)::pragmas";
			}
		}
		if (new_list->size() != 0) {
			set_pragmas(new_list);
		}
		else {
			set_pragmas(nullptr);
		}
	}
	if (get_statements()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_statements()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Pragmalist::replace matches multiple times (list(Node)::statements)";
					}
					if(new_node) {
						new_list->push_back(new_node);
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Pragmalist::replace "
								<< "of children list(Node)::statements";
			}
		}
		if (new_list->size() != 0) {
			set_statements(new_list);
		}
		else {
			set_statements(nullptr);
		}
	}
	return found;
}

bool Pragmalist::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_pragmas()) {
		Pragma::ListPtr new_list = std::make_shared<Pragma::List>();
		for (Pragma::Ptr lnode : *get_pragmas()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Pragmalist::replace matches multiple times (list(Pragma)::pragmas)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<Pragma>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Pragmalist::replace "
								<< "of children list(Pragma)::pragmas";
			}
		}
		if (new_list->size() != 0) {
			set_pragmas(new_list);
		}
		else {
			set_pragmas(nullptr);
		}
	}
	if (get_statements()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_statements()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Pragmalist::replace matches multiple times (list(Node)::statements)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(n);
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Pragmalist::replace "
								<< "of children list(Node)::statements";
			}
		}
		if (new_list->size() != 0) {
			set_statements(new_list);
		}
		else {
			set_statements(nullptr);
		}
	}
	return found;
}

Pragmalist::ListPtr Pragmalist::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Pragmalist>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Pragmalist::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_pragmas()) {
		for (const Pragma::Ptr node : *get_pragmas()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	if (get_statements()) {
		for (const Node::Ptr node : *get_statements()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void Pragmalist::clone_children(Node::Ptr new_node) const {
	cast_to<Pragmalist>(new_node)->set_pragmas
		(Pragma::clone_list(get_pragmas()));
	cast_to<Pragmalist>(new_node)->set_statements
		(Node::clone_list(get_statements()));
}

Node::Ptr Pragmalist::alloc_same(void) const {
	Ptr p(new Pragmalist);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Pragmalist &p) {
	os << "Pragmalist: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Pragmalist::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Pragmalist: {nullptr}";
	}

	return os;
}






}
}