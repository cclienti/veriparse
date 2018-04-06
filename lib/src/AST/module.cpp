#include <veriparse/AST/module.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Module::Module(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Module);
			set_node_categories({NodeType::Node});
		}

		
		Module::Module(const Parameter::ListPtr params, const Node::ListPtr ports, const Node::ListPtr items, const std::string &name, const Default_nettypeEnum &default_nettype, const std::string &filename, uint32_t line):
			Node(filename, line), m_params(params), m_ports(ports), m_items(items), m_name(name), m_default_nettype(default_nettype) {
			set_node_type(NodeType::Module);
			set_node_categories({NodeType::Node});
		}
		
		Module &Module::operator=(const Module &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			set_default_nettype(rhs.get_default_nettype());
			return *this;
		}

		Node &Module::operator=(const Node &rhs) {
			const Module &rhs_cast = static_cast<const Module&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Module::operator==(const Module &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			if (get_default_nettype() != rhs.get_default_nettype()) {
				return false;
			}
			return true;
		}

		bool Module::operator==(const Node &rhs) const {
			const Module &rhs_cast = static_cast<const Module&>(rhs);
			return operator==(rhs_cast);
		}

		bool Module::operator!=(const Module &rhs) const {
			return !(operator==(rhs));
		}

		bool Module::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Module::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Module::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_params()) {
				Parameter::ListPtr new_list = std::make_shared<Parameter::List>();
				for (Parameter::Ptr lnode : *get_params()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Module::replace matches multiple times (list(Parameter)::params)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Parameter>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Module::replace "
						            << "of children list(Parameter)::params";
					}
				}
				if (new_list->size() != 0) {
					set_params(new_list);
				}
				else {
					set_params(nullptr);
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
								            << "Module::replace matches multiple times (list(Node)::ports)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Module::replace "
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
			if (get_items()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_items()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Module::replace matches multiple times (list(Node)::items)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Module::replace "
						            << "of children list(Node)::items";
					}
				}
				if (new_list->size() != 0) {
					set_items(new_list);
				}
				else {
					set_items(nullptr);
				}
			}
			return found;
		}

		bool Module::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_params()) {
				Parameter::ListPtr new_list = std::make_shared<Parameter::List>();
				for (Parameter::Ptr lnode : *get_params()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Module::replace matches multiple times (list(Parameter)::params)";
							}
							if(new_nodes) {
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(cast_to<Parameter>(n));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Module::replace "
						            << "of children list(Parameter)::params";
					}
				}
				if (new_list->size() != 0) {
					set_params(new_list);
				}
				else {
					set_params(nullptr);
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
								            << "Module::replace matches multiple times (list(Node)::ports)";
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
						            << "found an empty node during Module::replace "
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
			if (get_items()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_items()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Module::replace matches multiple times (list(Node)::items)";
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
						            << "found an empty node during Module::replace "
						            << "of children list(Node)::items";
					}
				}
				if (new_list->size() != 0) {
					set_items(new_list);
				}
				else {
					set_items(nullptr);
				}
			}
			return found;
		}

		Module::ListPtr Module::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Module>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Module::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_params()) {
				for (const Parameter::Ptr node : *get_params()) {
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
			if (get_items()) {
				for (const Node::Ptr node : *get_items()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Module::clone_children(Node::Ptr new_node) const {
			cast_to<Module>(new_node)->set_params
				(Parameter::clone_list(get_params()));
			cast_to<Module>(new_node)->set_ports
				(Node::clone_list(get_ports()));
			cast_to<Module>(new_node)->set_items
				(Node::clone_list(get_items()));
		}

		Node::Ptr Module::alloc_same(void) const {
			Ptr p(new Module);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Module &p) {
			os << "Module: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name()<< ", ";
			
			os << "default_nettype: " << p.get_default_nettype();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Module::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Module: {nullptr}";
			}

			return os;
		}


		
		std::ostream & operator<<(std::ostream &os, const Module::Default_nettypeEnum p) {
			switch(p) {
			case Module::Default_nettypeEnum::INTEGER: os << "INTEGER"; break;
			case Module::Default_nettypeEnum::REAL: os << "REAL"; break;
			case Module::Default_nettypeEnum::REG: os << "REG"; break;
			case Module::Default_nettypeEnum::TRI: os << "TRI"; break;
			case Module::Default_nettypeEnum::WIRE: os << "WIRE"; break;
			case Module::Default_nettypeEnum::SUPPLY0: os << "SUPPLY0"; break;
			case Module::Default_nettypeEnum::SUPPLY1: os << "SUPPLY1"; break;
			case Module::Default_nettypeEnum::NONE: os << "NONE"; break;
			default: break;
			}
			return os;
		}

		



	}
}