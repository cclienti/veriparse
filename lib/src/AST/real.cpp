#include <veriparse/AST/real.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Real::Real(const std::string &filename, uint32_t line):
			Variable(filename, line)	{
			set_node_type(NodeType::Real);
			set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}

		
		Real::Real(const Length::ListPtr lengths, const Rvalue::Ptr right, const std::string &name, const std::string &filename, uint32_t line):
			Variable(lengths, right, name, filename, line) {
			set_node_type(NodeType::Real);
			set_node_categories({NodeType::Variable, NodeType::VariableBase, NodeType::Node});
		}
		
		Real &Real::operator=(const Real &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &Real::operator=(const Node &rhs) {
			const Real &rhs_cast = static_cast<const Real&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Real::operator==(const Real &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool Real::operator==(const Node &rhs) const {
			const Real &rhs_cast = static_cast<const Real&>(rhs);
			return operator==(rhs_cast);
		}

		bool Real::operator!=(const Real &rhs) const {
			return !(operator==(rhs));
		}

		bool Real::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Real::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Real::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
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
								            << "Real::replace matches multiple times (list(Length)::lengths)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Length>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Real::replace "
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
						            << "Real::replace matches multiple times (Rvalue::right)";
					}
					set_right(cast_to<Rvalue>(new_node));
					found = true;
				}
			}
			return found;
		}

		bool Real::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
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
								            << "Real::replace matches multiple times (list(Length)::lengths)";
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
						            << "found an empty node during Real::replace "
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

		Real::ListPtr Real::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Real>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Real::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
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

		void Real::clone_children(Node::Ptr new_node) const {
			cast_to<Real>(new_node)->set_lengths
				(Length::clone_list(get_lengths()));
			if (get_right()) {
				cast_to<Real>(new_node)->set_right
					(cast_to<Rvalue>(get_right()->clone()));
			}
		}

		Node::Ptr Real::alloc_same(void) const {
			Ptr p(new Real);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Real &p) {
			os << "Real: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Real::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Real: {nullptr}";
			}

			return os;
		}


		



	}
}