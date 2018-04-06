#include <veriparse/AST/lconcat.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Lconcat::Lconcat(const std::string &filename, uint32_t line):
			Concat(filename, line)	{
			set_node_type(NodeType::Lconcat);
			set_node_categories({NodeType::Concat, NodeType::Node});
		}

		
		Lconcat::Lconcat(const Node::ListPtr list, const std::string &filename, uint32_t line):
			Concat(list, filename, line) {
			set_node_type(NodeType::Lconcat);
			set_node_categories({NodeType::Concat, NodeType::Node});
		}
		
		Lconcat &Lconcat::operator=(const Lconcat &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Lconcat::operator=(const Node &rhs) {
			const Lconcat &rhs_cast = static_cast<const Lconcat&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Lconcat::operator==(const Lconcat &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Lconcat::operator==(const Node &rhs) const {
			const Lconcat &rhs_cast = static_cast<const Lconcat&>(rhs);
			return operator==(rhs_cast);
		}

		bool Lconcat::operator!=(const Lconcat &rhs) const {
			return !(operator==(rhs));
		}

		bool Lconcat::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Lconcat::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Lconcat::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_list()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_list()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Lconcat::replace matches multiple times (list(Node)::list)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Lconcat::replace "
						            << "of children list(Node)::list";
					}
				}
				if (new_list->size() != 0) {
					set_list(new_list);
				}
				else {
					set_list(nullptr);
				}
			}
			return found;
		}

		bool Lconcat::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_list()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_list()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Lconcat::replace matches multiple times (list(Node)::list)";
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
						            << "found an empty node during Lconcat::replace "
						            << "of children list(Node)::list";
					}
				}
				if (new_list->size() != 0) {
					set_list(new_list);
				}
				else {
					set_list(nullptr);
				}
			}
			return found;
		}

		Lconcat::ListPtr Lconcat::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Lconcat>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Lconcat::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_list()) {
				for (const Node::Ptr node : *get_list()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Lconcat::clone_children(Node::Ptr new_node) const {
			cast_to<Lconcat>(new_node)->set_list
				(Node::clone_list(get_list()));
		}

		Node::Ptr Lconcat::alloc_same(void) const {
			Ptr p(new Lconcat);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Lconcat &p) {
			os << "Lconcat: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Lconcat::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Lconcat: {nullptr}";
			}

			return os;
		}


		



	}
}