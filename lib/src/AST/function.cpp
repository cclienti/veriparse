#include <veriparse/AST/function.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

Function::Function(const std::string &filename, uint32_t line):
	Node(filename, line)	{
	set_node_type(NodeType::Function);
	set_node_categories({NodeType::Node});
}


Function::Function(const Width::ListPtr retwidths, const Node::ListPtr ports, const Node::ListPtr statements, const std::string &name, const bool &automatic, const RettypeEnum &rettype, const bool &retsign, const std::string &filename, uint32_t line):
	Node(filename, line), m_retwidths(retwidths), m_ports(ports), m_statements(statements), m_name(name), m_automatic(automatic), m_rettype(rettype), m_retsign(retsign) {
	set_node_type(NodeType::Function);
	set_node_categories({NodeType::Node});
}

Function &Function::operator=(const Function &rhs) {
	Node::operator=(static_cast<const Node &>(rhs));
	set_name(rhs.get_name());
	set_automatic(rhs.get_automatic());
	set_rettype(rhs.get_rettype());
	set_retsign(rhs.get_retsign());
	return *this;
}

Node &Function::operator=(const Node &rhs) {
	const Function &rhs_cast = static_cast<const Function&>(rhs);
	return static_cast<Node &> (operator=(rhs_cast));
}

bool Function::operator==(const Function &rhs) const {
	if (Node::operator==(rhs) == false) {
		return false;
	}
	if (get_name() != rhs.get_name()) {
		return false;
	}
	if (get_automatic() != rhs.get_automatic()) {
		return false;
	}
	if (get_rettype() != rhs.get_rettype()) {
		return false;
	}
	if (get_retsign() != rhs.get_retsign()) {
		return false;
	}
	return true;
}

bool Function::operator==(const Node &rhs) const {
	const Function &rhs_cast = static_cast<const Function&>(rhs);
	return operator==(rhs_cast);
}

bool Function::operator!=(const Function &rhs) const {
	return !(operator==(rhs));
}

bool Function::operator!=(const Node &rhs) const {
	return !(operator==(rhs));
}

bool Function::remove(Node::Ptr node) {
	return replace(node, AST::Node::Ptr(nullptr));
}

bool Function::replace(Node::Ptr node, Node::Ptr new_node) {
	bool found = false;
	if (get_retwidths()) {
		Width::ListPtr new_list = std::make_shared<Width::List>();
		for (Width::Ptr lnode : *get_retwidths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Function::replace matches multiple times (list(Width)::retwidths)";
					}
					if(new_node) {
						new_list->push_back(cast_to<Width>(new_node));
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Function::replace "
								<< "of children list(Width)::retwidths";
			}
		}
		if (new_list->size() != 0) {
			set_retwidths(new_list);
		}
		else {
			set_retwidths(nullptr);
		}
	}
	if (get_ports()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_ports()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Function::replace matches multiple times (list(Node)::ports)";
					}
					if(new_node) {
						new_list->push_back(new_node);
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Function::replace "
								<< "of children list(Node)::ports";
			}
		}
		if (new_list->size() != 0) {
			set_ports(new_list);
		}
		else {
			set_ports(nullptr);
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
										<< "Function::replace matches multiple times (list(Node)::statements)";
					}
					if(new_node) {
						new_list->push_back(new_node);
					}
					found = true;
				}
			}
			else {
				LOG_WARNING << *this << ", "
								<< "found an empty node during Function::replace "
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

bool Function::replace(Node::Ptr node, Node::ListPtr new_nodes) {
	bool found = false;
	if (get_retwidths()) {
		Width::ListPtr new_list = std::make_shared<Width::List>();
		for (Width::Ptr lnode : *get_retwidths()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Function::replace matches multiple times (list(Width)::retwidths)";
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
								<< "found an empty node during Function::replace "
								<< "of children list(Width)::retwidths";
			}
		}
		if (new_list->size() != 0) {
			set_retwidths(new_list);
		}
		else {
			set_retwidths(nullptr);
		}
	}
	if (get_ports()) {
		Node::ListPtr new_list = std::make_shared<Node::List>();
		for (Node::Ptr lnode : *get_ports()) {
			if (lnode) {
				if (lnode != node) {
					new_list->push_back(lnode);
				}
				else {
					if (found) {
						LOG_WARNING << *this << ", "
										<< "Function::replace matches multiple times (list(Node)::ports)";
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
								<< "found an empty node during Function::replace "
								<< "of children list(Node)::ports";
			}
		}
		if (new_list->size() != 0) {
			set_ports(new_list);
		}
		else {
			set_ports(nullptr);
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
										<< "Function::replace matches multiple times (list(Node)::statements)";
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
								<< "found an empty node during Function::replace "
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

Function::ListPtr Function::clone_list(const ListPtr nodes) {
	ListPtr list;
	if (nodes) {
			 list = std::make_shared<List>();
		for(const Ptr p : *nodes) {
			list->push_back(cast_to<Function>(p->clone()));
		}
	}
	return list;
}

Node::ListPtr Function::get_children(void) const {
	Node::ListPtr list = std::make_shared<Node::List>();
	if (get_retwidths()) {
		for (const Width::Ptr node : *get_retwidths()) {
			if (node) {
				list->push_back(std::static_pointer_cast<Node>(node));
			}
		}
	}
	if (get_ports()) {
		for (const Node::Ptr node : *get_ports()) {
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

void Function::clone_children(Node::Ptr new_node) const {
	cast_to<Function>(new_node)->set_retwidths
		(Width::clone_list(get_retwidths()));
	cast_to<Function>(new_node)->set_ports
		(Node::clone_list(get_ports()));
	cast_to<Function>(new_node)->set_statements
		(Node::clone_list(get_statements()));
}

Node::Ptr Function::alloc_same(void) const {
	Ptr p(new Function);
	return p;
}


std::ostream & operator<<(std::ostream &os, const Function &p) {
	os << "Function: {";
	if (!p.get_filename().empty()) {
		os << "filename: " << p.get_filename() << ", "
			<< "line: " << p.get_line();
	}
	
	if (!p.get_filename().empty()) os << ", ";
	
	os << "name: " << p.get_name()<< ", ";
	
	os << "automatic: " << p.get_automatic()<< ", ";
	
	os << "rettype: " << p.get_rettype()<< ", ";
	
	os << "retsign: " << p.get_retsign();
	os << "}";
	return os;
}


std::ostream & operator<<(std::ostream &os, const Function::Ptr p) {
	if (p) {
		os << *p;
	}
	else {
		os << "Function: {nullptr}";
	}

	return os;
}



std::ostream & operator<<(std::ostream &os, const Function::RettypeEnum p) {
	switch(p) {
	case Function::RettypeEnum::INTEGER: os << "INTEGER"; break;
	case Function::RettypeEnum::REAL: os << "REAL"; break;
	case Function::RettypeEnum::NONE: os << "NONE"; break;
	default: break;
	}
	return os;
}





}
}