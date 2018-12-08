#include <veriparse/AST/variable.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Variable::Variable(const std::string &filename, uint32_t line):
	VariableBase(filename, line)	{
	set_node_type(NodeType::Variable);
	set_node_categories({NodeType::VariableBase, NodeType::Node});
}


Variable::Variable(const Length::ListPtr lengths, const Rvalue::Ptr right, const std::string &name, const std::string &filename, uint32_t line):
	VariableBase(name, filename, line), m_lengths(lengths), m_right(right) {
	set_node_type(NodeType::Variable);
	set_node_categories({NodeType::VariableBase, NodeType::Node});
}

Variable &Variable::operator=(const Variable &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	return *this;
}

Node &Variable::operator=(const Node &rhs) {
	const Variable &rhs_cast = static_cast<const Variable&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Variable::operator==(const Variable &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	return true;
}

bool Variable::operator==(const Node &rhs) const {
	const Variable &rhs_cast = static_cast<const Variable&>(rhs);
	return operator==(rhs_cast);
}

bool Variable::operator!=(const Variable &rhs) const {
	return !(operator==(rhs));
}

bool Variable::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Variable::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Variable::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_lengths()) {
		Length::ListPtr new_list = std::make_shared<Length::List>();
		for (Length::Ptr lnode : *get_lengths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Variable::replace matches multiple times (list(Length)::lengths)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Length>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Variable::replace "
								<< "of children list(Length)::lengths";
			}
		}
		if (new_list->size() != 0) {
			set_lengths(new_list);
		}
		else {
			set_lengths(nullptr);
		}
	}
	if (get_right()) {
		if (get_right() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Variable::replace matches multiple times (Rvalue::right)";
			}
			set_right(cast_to<Rvalue>(new_node));
			found = true;
		}
	}
	return found;
}

bool Variable::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_lengths()) {
		Length::ListPtr new_list = std::make_shared<Length::List>();
		for (Length::Ptr lnode : *get_lengths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Variable::replace matches multiple times (list(Length)::lengths)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<Length>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Variable::replace "
								<< "of children list(Length)::lengths";
			}
		}
		if (new_list->size() != 0) {
			set_lengths(new_list);
		}
		else {
			set_lengths(nullptr);
		}
	}
	return found;
}

Variable::ListPtr Variable::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Variable>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Variable::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_lengths()) {
		for (const Length::Ptr node : *get_lengths()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	if (get_right()) {
		list->push_back(std::static_pointer_cast<Node>(get_right()));
	}
	return list;
}

void Variable::clone_children(Node::Ptr new_node) const {
	cast_to<Variable>(new_node)->set_lengths
		(Length::clone_list(get_lengths()));
	if (get_right()) {
		cast_to<Variable>(new_node)->set_right
			(cast_to<Rvalue>(get_right()->clone()));
	}
}

Node::Ptr Variable::alloc_same(void) const {
	Ptr p(new Variable);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Variable &p) {
	os << "Variable: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Variable::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Variable: {nullptr}";
	}

	return os;
}






}
}