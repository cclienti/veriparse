#include <veriparse/AST/inout.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Inout::Inout(const std::string &filename, uint32_t line):
			IODir(filename, line)	{
			set_node_type(NodeType::Inout);
			set_node_categories({NodeType::IODir, NodeType::Node});
		}

		
		Inout::Inout(const Width::ListPtr widths, const std::string &name, const bool &sign, const std::string &filename, uint32_t line):
			IODir(widths, name, sign, filename, line) {
			set_node_type(NodeType::Inout);
			set_node_categories({NodeType::IODir, NodeType::Node});
		}
		
		Inout &Inout::operator=(const Inout &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			set_sign(rhs.get_sign());
			return *this;
		}

		Node &Inout::operator=(const Node &rhs) {
			const Inout &rhs_cast = static_cast<const Inout&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Inout::operator==(const Inout &rhs) const {
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

		bool Inout::operator==(const Node &rhs) const {
			const Inout &rhs_cast = static_cast<const Inout&>(rhs);
			return operator==(rhs_cast);
		}

		bool Inout::operator!=(const Inout &rhs) const {
			return !(operator==(rhs));
		}

		bool Inout::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Inout::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Inout::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "Inout::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Inout::replace "
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

		bool Inout::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Inout::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Inout::replace "
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

		Inout::ListPtr Inout::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Inout>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Inout::get_children(void) const {
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

		void Inout::clone_children(Node::Ptr new_node) const {
			cast_to<Inout>(new_node)->set_widths
				(Width::clone_list(get_widths()));
		}

		Node::Ptr Inout::alloc_same(void) const {
			Ptr p(new Inout);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Inout &p) {
			os << "Inout: {";
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


		std::ostream & operator<<(std::ostream &os, const Inout::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Inout: {nullptr}";
			}

			return os;
		}


		



	}
}