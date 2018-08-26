#include <veriparse/AST/localparam.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Localparam::Localparam(const std::string &filename, uint32_t line):
			Parameter(filename, line)	{
			set_node_type(NodeType::Localparam);
			set_node_categories({NodeType::Parameter, NodeType::Node});
		}

		
		Localparam::Localparam(const Node::Ptr value, const Width::ListPtr widths, const std::string &name, const bool &sign, const Parameter::TypeEnum &type, const std::string &filename, uint32_t line):
			Parameter(value, widths, name, sign, type, filename, line) {
			set_node_type(NodeType::Localparam);
			set_node_categories({NodeType::Parameter, NodeType::Node});
		}
		
		Localparam &Localparam::operator=(const Localparam &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			set_sign(rhs.get_sign());
			set_type(rhs.get_type());
			return *this;
		}

		Node &Localparam::operator=(const Node &rhs) {
			const Localparam &rhs_cast = static_cast<const Localparam&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Localparam::operator==(const Localparam &rhs) const {
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

		bool Localparam::operator==(const Node &rhs) const {
			const Localparam &rhs_cast = static_cast<const Localparam&>(rhs);
			return operator==(rhs_cast);
		}

		bool Localparam::operator!=(const Localparam &rhs) const {
			return !(operator==(rhs));
		}

		bool Localparam::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Localparam::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Localparam::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_value()) {
				if (get_value() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Localparam::replace matches multiple times (Node::value)";
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
								            << "Localparam::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Localparam::replace "
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

		bool Localparam::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Localparam::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Localparam::replace "
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

		Localparam::ListPtr Localparam::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Localparam>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Localparam::get_children(void) const {
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

		void Localparam::clone_children(Node::Ptr new_node) const {
			if (get_value()) {
				cast_to<Localparam>(new_node)->set_value
					(get_value()->clone());
			}
			cast_to<Localparam>(new_node)->set_widths
				(Width::clone_list(get_widths()));
		}

		Node::Ptr Localparam::alloc_same(void) const {
			Ptr p(new Localparam);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Localparam &p) {
			os << "Localparam: {";
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


		std::ostream & operator<<(std::ostream &os, const Localparam::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Localparam: {nullptr}";
			}

			return os;
		}


		



	}
}