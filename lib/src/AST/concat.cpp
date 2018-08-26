#include <veriparse/AST/concat.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Concat::Concat(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Concat);
			set_node_categories({NodeType::Node});
		}

		
		Concat::Concat(const Node::ListPtr list, const std::string &filename, uint32_t line):
			Node(filename, line), m_list(list) {
			set_node_type(NodeType::Concat);
			set_node_categories({NodeType::Node});
		}
		
		Concat &Concat::operator=(const Concat &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Concat::operator=(const Node &rhs) {
			const Concat &rhs_cast = static_cast<const Concat&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Concat::operator==(const Concat &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Concat::operator==(const Node &rhs) const {
			const Concat &rhs_cast = static_cast<const Concat&>(rhs);
			return operator==(rhs_cast);
		}

		bool Concat::operator!=(const Concat &rhs) const {
			return !(operator==(rhs));
		}

		bool Concat::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Concat::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Concat::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "Concat::replace matches multiple times (list(Node)::list)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Concat::replace "
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

		bool Concat::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "Concat::replace matches multiple times (list(Node)::list)";
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
						            << "found an empty node during Concat::replace "
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

		Concat::ListPtr Concat::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Concat>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Concat::get_children(void) const {
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

		void Concat::clone_children(Node::Ptr new_node) const {
			cast_to<Concat>(new_node)->set_list
				(Node::clone_list(get_list()));
		}

		Node::Ptr Concat::alloc_same(void) const {
			Ptr p(new Concat);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Concat &p) {
			os << "Concat: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Concat::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Concat: {nullptr}";
			}

			return os;
		}


		



	}
}