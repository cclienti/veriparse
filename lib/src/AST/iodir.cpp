#include <veriparse/AST/iodir.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

IODir::IODir(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::IODir);
	set_node_categories({NodeType::Node});
}


IODir::IODir(const Width::ListPtr widths, const std::string &name, const bool &sign, const std::string &filename, uint32_t line):
	Node(filename, line), m_widths(widths), m_name(name), m_sign(sign) {
	set_node_type(NodeType::IODir);
	set_node_categories({NodeType::Node});
}

IODir &IODir::operator=(const IODir &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	set_sign(rhs.get_sign());
	return *this;
}

Node &IODir::operator=(const Node &rhs) {
	const IODir &rhs_cast = static_cast<const IODir&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool IODir::operator==(const IODir &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	if (get_sign() != rhs.get_sign()) {
		return false;
	}
	return true;
}

bool IODir::operator==(const Node &rhs) const {
	const IODir &rhs_cast = static_cast<const IODir&>(rhs);
	return operator==(rhs_cast);
}

bool IODir::operator!=(const IODir &rhs) const {
	return !(operator==(rhs));
}

bool IODir::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool IODir::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool IODir::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_widths()) {
		Width::ListPtr new_list = std::make_shared<Width::List>();
		for (Width::Ptr lnode : *get_widths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "IODir::replace matches multiple times (list(Width)::widths)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Width>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during IODir::replace "
								<< "of children list(Width)::widths";
			}
		}
		if (new_list->size() != 0) {
			set_widths(new_list);
		}
		else {
			set_widths(nullptr);
		}
	}
	return found;
}

bool IODir::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_widths()) {
		Width::ListPtr new_list = std::make_shared<Width::List>();
		for (Width::Ptr lnode : *get_widths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "IODir::replace matches multiple times (list(Width)::widths)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<Width>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during IODir::replace "
								<< "of children list(Width)::widths";
			}
		}
		if (new_list->size() != 0) {
			set_widths(new_list);
		}
		else {
			set_widths(nullptr);
		}
	}
	return found;
}

IODir::ListPtr IODir::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<IODir>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr IODir::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_widths()) {
		for (const Width::Ptr node : *get_widths()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void IODir::clone_children(Node::Ptr new_node) const {
	cast_to<IODir>(new_node)->set_widths
		(Width::clone_list(get_widths()));
}

Node::Ptr IODir::alloc_same(void) const {
	Ptr p(new IODir);
	return p;
}


std::ostream & operator<<(std::ostream &os, const IODir &p) {
	os << "IODir: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name()<< ", ";
	
	os << "sign: " << p.get_sign();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const IODir::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "IODir: {nullptr}";
	}

	return os;
}






}
}