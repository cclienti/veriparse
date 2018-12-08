#include <veriparse/AST/instance.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Instance::Instance(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Instance);
	set_node_categories({NodeType::Node});
}


Instance::Instance(const Length::Ptr array, const ParamArg::ListPtr parameterlist, const PortArg::ListPtr portlist, const std::string &module, const std::string &name, const std::string &filename, uint32_t line):
	Node(filename, line), m_array(array), m_parameterlist(parameterlist), m_portlist(portlist), m_module(module), m_name(name) {
	set_node_type(NodeType::Instance);
	set_node_categories({NodeType::Node});
}

Instance &Instance::operator=(const Instance &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_module(rhs.get_module());
	set_name(rhs.get_name());
	return *this;
}

Node &Instance::operator=(const Node &rhs) {
	const Instance &rhs_cast = static_cast<const Instance&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Instance::operator==(const Instance &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_module() != rhs.get_module()) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	return true;
}

bool Instance::operator==(const Node &rhs) const {
	const Instance &rhs_cast = static_cast<const Instance&>(rhs);
	return operator==(rhs_cast);
}

bool Instance::operator!=(const Instance &rhs) const {
	return !(operator==(rhs));
}

bool Instance::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Instance::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Instance::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_array()) {
		if (get_array() == node) {
			if (found) {
				LOG_WARNING << *this << ", "
								<< "Instance::replace matches multiple times (Length::array)";
			}
			set_array(cast_to<Length>(new_node));
			found = true;
		}
	}
	if (get_parameterlist()) {
		ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
		for (ParamArg::Ptr lnode : *get_parameterlist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instance::replace matches multiple times (list(ParamArg)::parameterlist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<ParamArg>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instance::replace "
								<< "of children list(ParamArg)::parameterlist";
			}
		}
		if (new_list->size() != 0) {
			set_parameterlist(new_list);
		}
		else {
			set_parameterlist(nullptr);
		}
	}
	if (get_portlist()) {
		PortArg::ListPtr new_list = std::make_shared<PortArg::List>();
		for (PortArg::Ptr lnode : *get_portlist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instance::replace matches multiple times (list(PortArg)::portlist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<PortArg>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instance::replace "
								<< "of children list(PortArg)::portlist";
			}
		}
		if (new_list->size() != 0) {
			set_portlist(new_list);
		}
		else {
			set_portlist(nullptr);
		}
	}
	return found;
}

bool Instance::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_parameterlist()) {
		ParamArg::ListPtr new_list = std::make_shared<ParamArg::List>();
		for (ParamArg::Ptr lnode : *get_parameterlist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instance::replace matches multiple times (list(ParamArg)::parameterlist)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<ParamArg>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instance::replace "
								<< "of children list(ParamArg)::parameterlist";
			}
		}
		if (new_list->size() != 0) {
			set_parameterlist(new_list);
		}
		else {
			set_parameterlist(nullptr);
		}
	}
	if (get_portlist()) {
		PortArg::ListPtr new_list = std::make_shared<PortArg::List>();
		for (PortArg::Ptr lnode : *get_portlist()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instance::replace matches multiple times (list(PortArg)::portlist)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<PortArg>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instance::replace "
								<< "of children list(PortArg)::portlist";
			}
		}
		if (new_list->size() != 0) {
			set_portlist(new_list);
		}
		else {
			set_portlist(nullptr);
		}
	}
	return found;
}

Instance::ListPtr Instance::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Instance>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Instance::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_array()) {
		list->push_back(std::static_pointer_cast<Node>(get_array()));
	}
	if (get_parameterlist()) {
		for (const ParamArg::Ptr node : *get_parameterlist()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	if (get_portlist()) {
		for (const PortArg::Ptr node : *get_portlist()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void Instance::clone_children(Node::Ptr new_node) const {
	if (get_array()) {
		cast_to<Instance>(new_node)->set_array
			(cast_to<Length>(get_array()->clone()));
	}
	cast_to<Instance>(new_node)->set_parameterlist
		(ParamArg::clone_list(get_parameterlist()));
	cast_to<Instance>(new_node)->set_portlist
		(PortArg::clone_list(get_portlist()));
}

Node::Ptr Instance::alloc_same(void) const {
	Ptr p(new Instance);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Instance &p) {
	os << "Instance: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "module: " << p.get_module()<< ", ";
	
	os << "name: " << p.get_name();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Instance::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Instance: {nullptr}";
	}

	return os;
}






}
}