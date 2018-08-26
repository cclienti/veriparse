#include <veriparse/AST/senslist.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Senslist::Senslist(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Senslist);
			set_node_categories({NodeType::Node});
		}

		
		Senslist::Senslist(const Sens::ListPtr list, const std::string &filename, uint32_t line):
			Node(filename, line), m_list(list) {
			set_node_type(NodeType::Senslist);
			set_node_categories({NodeType::Node});
		}
		
		Senslist &Senslist::operator=(const Senslist &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Senslist::operator=(const Node &rhs) {
			const Senslist &rhs_cast = static_cast<const Senslist&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Senslist::operator==(const Senslist &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Senslist::operator==(const Node &rhs) const {
			const Senslist &rhs_cast = static_cast<const Senslist&>(rhs);
			return operator==(rhs_cast);
		}

		bool Senslist::operator!=(const Senslist &rhs) const {
			return !(operator==(rhs));
		}

		bool Senslist::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Senslist::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Senslist::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_list()) {
				Sens::ListPtr new_list = std::make_shared<Sens::List>();
				for (Sens::Ptr lnode : *get_list()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Senslist::replace matches multiple times (list(Sens)::list)";
							}
							if(new_node) {
								new_list->push_back(cast_to<Sens>(new_node));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Senslist::replace "
						            << "of children list(Sens)::list";
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

		bool Senslist::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_list()) {
				Sens::ListPtr new_list = std::make_shared<Sens::List>();
				for (Sens::Ptr lnode : *get_list()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Senslist::replace matches multiple times (list(Sens)::list)";
							}
							if(new_nodes) {
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(cast_to<Sens>(n));
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Senslist::replace "
						            << "of children list(Sens)::list";
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

		Senslist::ListPtr Senslist::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Senslist>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Senslist::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_list()) {
				for (const Sens::Ptr node : *get_list()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Senslist::clone_children(Node::Ptr new_node) const {
			cast_to<Senslist>(new_node)->set_list
				(Sens::clone_list(get_list()));
		}

		Node::Ptr Senslist::alloc_same(void) const {
			Ptr p(new Senslist);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Senslist &p) {
			os << "Senslist: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Senslist::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Senslist: {nullptr}";
			}

			return os;
		}


		



	}
}