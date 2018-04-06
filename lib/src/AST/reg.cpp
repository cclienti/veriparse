#include <veriparse/AST/reg.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Reg::Reg(const std::string &filename, uint32_t line):
			Variable(filename, line)	{
			set_node_type(NodeType::Reg);
			set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}

		
		Reg::Reg(const Width::ListPtr widths, const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign, const std::string &name, const std::string &filename, uint32_t line):
			Variable(lengths, right, name, filename, line), m_widths(widths), m_sign(sign) {
			set_node_type(NodeType::Reg);
			set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}
		
		Reg &Reg::operator=(const Reg &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_sign(rhs.get_sign());
			set_name(rhs.get_name());
			return *this;
		}

		Node &Reg::operator=(const Node &rhs) {
			const Reg &rhs_cast = static_cast<const Reg&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Reg::operator==(const Reg &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_sign() != rhs.get_sign()) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool Reg::operator==(const Node &rhs) const {
			const Reg &rhs_cast = static_cast<const Reg&>(rhs);
			return operator==(rhs_cast);
		}

		bool Reg::operator!=(const Reg &rhs) const {
			return !(operator==(rhs));
		}

		bool Reg::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Reg::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Reg::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "Reg::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Reg::replace "
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
			if (get_lengths()) {
				Length::ListPtr new_list = std::make_shared<Length::List>();
				for (Length::Ptr lnode : *get_lengths()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Reg::replace matches multiple times (list(Length)::lengths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Length>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Reg::replace "
						            << "of children list(Length)::lengths";
					}
				}
				if (new_list->size() != 0) {
					set_lengths(new_list);
				}
				else {
					set_lengths(nullptr);
				}
			}
			if (get_right()) {
				if (get_right() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Reg::replace matches multiple times (Rvalue::right)";
					}
					set_right(cast_to<Rvalue>(new_node));
					found = true;
				}
			}
			return found;
		}

		bool Reg::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Reg::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Reg::replace "
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
			if (get_lengths()) {
				Length::ListPtr new_list = std::make_shared<Length::List>();
				for (Length::Ptr lnode : *get_lengths()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Reg::replace matches multiple times (list(Length)::lengths)";
							}
							if(new_nodes) {
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(cast_to<Length>(n));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Reg::replace "
						            << "of children list(Length)::lengths";
					}
				}
				if (new_list->size() != 0) {
					set_lengths(new_list);
				}
				else {
					set_lengths(nullptr);
				}
			}
			return found;
		}

		Reg::ListPtr Reg::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Reg>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Reg::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_widths()) {
				for (const Width::Ptr node : *get_widths()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			if (get_lengths()) {
				for (const Length::Ptr node : *get_lengths()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			if (get_right()) {
				list->push_back(std::static_pointer_cast<Node>(get_right()));
			}
			return list;
		}

		void Reg::clone_children(Node::Ptr new_node) const {
			cast_to<Reg>(new_node)->set_widths
				(Width::clone_list(get_widths()));
			cast_to<Reg>(new_node)->set_lengths
				(Length::clone_list(get_lengths()));
			if (get_right()) {
				cast_to<Reg>(new_node)->set_right
					(cast_to<Rvalue>(get_right()->clone()));
			}
		}

		Node::Ptr Reg::alloc_same(void) const {
			Ptr p(new Reg);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Reg &p) {
			os << "Reg: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "sign: " << p.get_sign()<< ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Reg::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Reg: {nullptr}";
			}

			return os;
		}


		



	}
}