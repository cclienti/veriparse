#include <veriparse/AST/systemcall.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

SystemCall::SystemCall(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::SystemCall);
	set_node_categories({NodeType::Node});
}


SystemCall::SystemCall(const Node::ListPtr args, const std::string &syscall, const std::string &filename, uint32_t line):
	Node(filename, line), m_args(args), m_syscall(syscall) {
	set_node_type(NodeType::SystemCall);
	set_node_categories({NodeType::Node});
}

SystemCall &SystemCall::operator=(const SystemCall &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_syscall(rhs.get_syscall());
	return *this;
}

Node &SystemCall::operator=(const Node &rhs) {
	const SystemCall &rhs_cast = static_cast<const SystemCall&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool SystemCall::operator==(const SystemCall &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_syscall() != rhs.get_syscall()) {
		return false;
	}
	return true;
}

bool SystemCall::operator==(const Node &rhs) const {
	const SystemCall &rhs_cast = static_cast<const SystemCall&>(rhs);
	return operator==(rhs_cast);
}

bool SystemCall::operator!=(const SystemCall &rhs) const {
	return !(operator==(rhs));
}

bool SystemCall::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool SystemCall::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool SystemCall::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_args()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_args()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "SystemCall::replace matches multiple times (list(Node)::args)";
					}
					if(new_node) {
						new_list->push_back(new_node);
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during SystemCall::replace "
								<< "of children list(Node)::args";
			}
		}
		if (new_list->size() != 0) {
			set_args(new_list);
		}
		else {
			set_args(nullptr);
		}
	}
	return found;
}

bool SystemCall::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_args()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_args()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "SystemCall::replace matches multiple times (list(Node)::args)";
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
								<< "found an empty node during SystemCall::replace "
								<< "of children list(Node)::args";
			}
		}
		if (new_list->size() != 0) {
			set_args(new_list);
		}
		else {
			set_args(nullptr);
		}
	}
	return found;
}

SystemCall::ListPtr SystemCall::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<SystemCall>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr SystemCall::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_args()) {
		for (const Node::Ptr node : *get_args()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void SystemCall::clone_children(Node::Ptr new_node) const {
	cast_to<SystemCall>(new_node)->set_args
		(Node::clone_list(get_args()));
}

Node::Ptr SystemCall::alloc_same(void) const {
	Ptr p(new SystemCall);
	return p;
}


std::ostream & operator<<(std::ostream &os, const SystemCall &p) {
	os << "SystemCall: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "syscall: " << p.get_syscall();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const SystemCall::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "SystemCall: {nullptr}";
	}

	return os;
}






}
}