#include <veriparse/AST/instancelist.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Instancelist::Instancelist(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Instancelist);
	set_node_categories({NodeType::Node});
}


Instancelist::Instancelist(const ParamArg::ListPtr parameterlist, const Instance::ListPtr instances, const std::string &module, const std::string &filename, uint32_t line):
	Node(filename, line), m_parameterlist(parameterlist), m_instances(instances), m_module(module) {
	set_node_type(NodeType::Instancelist);
	set_node_categories({NodeType::Node});
}

Instancelist &Instancelist::operator=(const Instancelist &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_module(rhs.get_module());
	return *this;
}

Node &Instancelist::operator=(const Node &rhs) {
	const Instancelist &rhs_cast = static_cast<const Instancelist&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Instancelist::operator==(const Instancelist &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_module() != rhs.get_module()) {
		return false;
	}
	return true;
}

bool Instancelist::operator==(const Node &rhs) const {
	const Instancelist &rhs_cast = static_cast<const Instancelist&>(rhs);
	return operator==(rhs_cast);
}

bool Instancelist::operator!=(const Instancelist &rhs) const {
	return !(operator==(rhs));
}

bool Instancelist::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Instancelist::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Instancelist::replace(Node::Ptr node, Node::Ptr new_node) {
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
										<< "Instancelist::replace matches multiple times (list(ParamArg)::parameterlist)";
					}
					if(new_node) {
						new_list->push_back(cast_to<ParamArg>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instancelist::replace "
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
	if (get_instances()) {
		Instance::ListPtr new_list = std::make_shared<Instance::List>();
		for (Instance::Ptr lnode : *get_instances()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instancelist::replace matches multiple times (list(Instance)::instances)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Instance>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instancelist::replace "
								<< "of children list(Instance)::instances";
			}
		}
		if (new_list->size() != 0) {
			set_instances(new_list);
		}
		else {
			set_instances(nullptr);
		}
	}
	return found;
}

bool Instancelist::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
										<< "Instancelist::replace matches multiple times (list(ParamArg)::parameterlist)";
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
								<< "found an empty node during Instancelist::replace "
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
	if (get_instances()) {
		Instance::ListPtr new_list = std::make_shared<Instance::List>();
		for (Instance::Ptr lnode : *get_instances()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Instancelist::replace matches multiple times (list(Instance)::instances)";
					}
					if(new_nodes) {
						for(Node::Ptr n: *new_nodes)
							new_list->push_back(cast_to<Instance>(n));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Instancelist::replace "
								<< "of children list(Instance)::instances";
			}
		}
		if (new_list->size() != 0) {
			set_instances(new_list);
		}
		else {
			set_instances(nullptr);
		}
	}
	return found;
}

Instancelist::ListPtr Instancelist::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Instancelist>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Instancelist::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_parameterlist()) {
		for (const ParamArg::Ptr node : *get_parameterlist()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	if (get_instances()) {
		for (const Instance::Ptr node : *get_instances()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	return list;
}

void Instancelist::clone_children(Node::Ptr new_node) const {
	cast_to<Instancelist>(new_node)->set_parameterlist
		(ParamArg::clone_list(get_parameterlist()));
	cast_to<Instancelist>(new_node)->set_instances
		(Instance::clone_list(get_instances()));
}

Node::Ptr Instancelist::alloc_same(void) const {
	Ptr p(new Instancelist);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Instancelist &p) {
	os << "Instancelist: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "module: " << p.get_module();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Instancelist::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Instancelist: {nullptr}";
	}

	return os;
}






}
}