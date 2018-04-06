#include <veriparse/AST/wire.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Wire::Wire(const std::string &filename, uint32_t line):
			Net(filename, line)	{
			set_node_type(NodeType::Wire);
			set_node_categories({NodeType::Net, NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}

		
		Wire::Wire(const Width::ListPtr widths, const DelayStatement::Ptr ldelay, const DelayStatement::Ptr rdelay, const Length::ListPtr lengths, const Rvalue::Ptr right, const bool &sign, const std::string &name, const std::string &filename, uint32_t line):
			Net(widths, ldelay, rdelay, lengths, right, sign, name, filename, line) {
			set_node_type(NodeType::Wire);
			set_node_categories({NodeType::Net, NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}
		
		Wire &Wire::operator=(const Wire &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_sign(rhs.get_sign());
			set_name(rhs.get_name());
			return *this;
		}

		Node &Wire::operator=(const Node &rhs) {
			const Wire &rhs_cast = static_cast<const Wire&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Wire::operator==(const Wire &rhs) const {
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

		bool Wire::operator==(const Node &rhs) const {
			const Wire &rhs_cast = static_cast<const Wire&>(rhs);
			return operator==(rhs_cast);
		}

		bool Wire::operator!=(const Wire &rhs) const {
			return !(operator==(rhs));
		}

		bool Wire::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Wire::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Wire::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "Wire::replace matches multiple times (list(Width)::widths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Width>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Wire::replace "
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
			if (get_ldelay()) {
				if (get_ldelay() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Wire::replace matches multiple times (DelayStatement::ldelay)";
					}
					set_ldelay(cast_to<DelayStatement>(new_node));
					found = true;
				}
			}
			if (get_rdelay()) {
				if (get_rdelay() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Wire::replace matches multiple times (DelayStatement::rdelay)";
					}
					set_rdelay(cast_to<DelayStatement>(new_node));
					found = true;
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
								            << "Wire::replace matches multiple times (list(Length)::lengths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Length>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Wire::replace "
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
						            << "Wire::replace matches multiple times (Rvalue::right)";
					}
					set_right(cast_to<Rvalue>(new_node));
					found = true;
				}
			}
			return found;
		}

		bool Wire::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Wire::replace matches multiple times (list(Width)::widths)";
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
						            << "found an empty node during Wire::replace "
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
								            << "Wire::replace matches multiple times (list(Length)::lengths)";
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
						            << "found an empty node during Wire::replace "
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

		Wire::ListPtr Wire::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Wire>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Wire::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_widths()) {
				for (const Width::Ptr node : *get_widths()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			if (get_ldelay()) {
				list->push_back(std::static_pointer_cast<Node>(get_ldelay()));
			}
			if (get_rdelay()) {
				list->push_back(std::static_pointer_cast<Node>(get_rdelay()));
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

		void Wire::clone_children(Node::Ptr new_node) const {
			cast_to<Wire>(new_node)->set_widths
				(Width::clone_list(get_widths()));
			if (get_ldelay()) {
				cast_to<Wire>(new_node)->set_ldelay
					(cast_to<DelayStatement>(get_ldelay()->clone()));
			}
			if (get_rdelay()) {
				cast_to<Wire>(new_node)->set_rdelay
					(cast_to<DelayStatement>(get_rdelay()->clone()));
			}
			cast_to<Wire>(new_node)->set_lengths
				(Length::clone_list(get_lengths()));
			if (get_right()) {
				cast_to<Wire>(new_node)->set_right
					(cast_to<Rvalue>(get_right()->clone()));
			}
		}

		Node::Ptr Wire::alloc_same(void) const {
			Ptr p(new Wire);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Wire &p) {
			os << "Wire: {";
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


		std::ostream & operator<<(std::ostream &os, const Wire::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Wire: {nullptr}";
			}

			return os;
		}


		



	}
}