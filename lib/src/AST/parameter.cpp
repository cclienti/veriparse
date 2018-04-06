#include <veriparse/AST/parameter.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Parameter::Parameter(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Parameter);
			set_node_categories({NodeType::Node});
		}

		
		Parameter::Parameter(const Node::Ptr value, const Width::ListPtr widths, const std::string &name, const bool &sign, const TypeEnum &type, const std::string &filename, uint32_t line):
			Node(filename, line), m_value(value), m_widths(widths), m_name(name), m_sign(sign), m_type(type) {
			set_node_type(NodeType::Parameter);
			set_node_categories({NodeType::Node});
		}
		
		Parameter &Parameter::operator=(const Parameter &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			set_sign(rhs.get_sign());
			set_type(rhs.get_type());
			return *this;
		}

		Node &Parameter::operator=(const Node &rhs) {
			const Parameter &rhs_cast = static_cast<const Parameter&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Parameter::operator==(const Parameter &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			if (get_sign() != rhs.get_sign()) {
				return false;
			}
			if (get_type() != rhs.get_type()) {
				return false;
			}
			return true;
		}

		bool Parameter::operator==(const Node &rhs) const {
			const Parameter &rhs_cast = static_cast<const Parameter&>(rhs);
			return operator==(rhs_cast);
		}

		bool Parameter::operator!=(const Parameter &rhs) const {
			return !(operator==(rhs));
		}

		bool Parameter::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Parameter::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Parameter::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_value()) {
				if (get_value() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Parameter::replace matches multiple times (Node::value)";
					}
					set_value(new_node);
					found = true;
				}
			}
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
								            << "Parameter::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Parameter::replace "
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

		bool Parameter::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Parameter::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Parameter::replace "
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

		Parameter::ListPtr Parameter::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Parameter>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Parameter::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_value()) {
				list->push_back(std::static_pointer_cast<Node>(get_value()));
			}
			if (get_widths()) {
				for (const Width::Ptr node : *get_widths()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Parameter::clone_children(Node::Ptr new_node) const {
			if (get_value()) {
				cast_to<Parameter>(new_node)->set_value
					(get_value()->clone());
			}
			cast_to<Parameter>(new_node)->set_widths
				(Width::clone_list(get_widths()));
		}

		Node::Ptr Parameter::alloc_same(void) const {
			Ptr p(new Parameter);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Parameter &p) {
			os << "Parameter: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name()<< ", ";
			
			os << "sign: " << p.get_sign()<< ", ";
			
			os << "type: " << p.get_type();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Parameter::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Parameter: {nullptr}";
			}

			return os;
		}


		
		std::ostream & operator<<(std::ostream &os, const Parameter::TypeEnum p) {
			switch(p) {
			case Parameter::TypeEnum::INTEGER: os << "INTEGER"; break;
			case Parameter::TypeEnum::REAL: os << "REAL"; break;
			case Parameter::TypeEnum::NONE: os << "NONE"; break;
			default: break;
			}
			return os;
		}

		



	}
}