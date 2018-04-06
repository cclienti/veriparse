#include <veriparse/AST/port.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Port::Port(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Port);
			set_node_categories({NodeType::Node});
		}

		
		Port::Port(const Width::ListPtr widths, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_widths(widths), m_name(name) {
			set_node_type(NodeType::Port);
			set_node_categories({NodeType::Node});
		}
		
		Port &Port::operator=(const Port &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &Port::operator=(const Node &rhs) {
			const Port &rhs_cast = static_cast<const Port&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Port::operator==(const Port &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool Port::operator==(const Node &rhs) const {
			const Port &rhs_cast = static_cast<const Port&>(rhs);
			return operator==(rhs_cast);
		}

		bool Port::operator!=(const Port &rhs) const {
			return !(operator==(rhs));
		}

		bool Port::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Port::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Port::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "Port::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Port::replace "
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

		bool Port::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Port::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Port::replace "
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

		Port::ListPtr Port::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Port>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Port::get_children(void) const {
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

		void Port::clone_children(Node::Ptr new_node) const {
			cast_to<Port>(new_node)->set_widths
				(Width::clone_list(get_widths()));
		}

		Node::Ptr Port::alloc_same(void) const {
			Ptr p(new Port);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Port &p) {
			os << "Port: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Port::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Port: {nullptr}";
			}

			return os;
		}


		



	}
}