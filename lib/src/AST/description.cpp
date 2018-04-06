#include <veriparse/AST/description.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Description::Description(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Description);
			set_node_categories({NodeType::Node});
		}

		
		Description::Description(const Node::ListPtr definitions, const std::string &filename, uint32_t line):
			Node(filename, line), m_definitions(definitions) {
			set_node_type(NodeType::Description);
			set_node_categories({NodeType::Node});
		}
		
		Description &Description::operator=(const Description &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Description::operator=(const Node &rhs) {
			const Description &rhs_cast = static_cast<const Description&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Description::operator==(const Description &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Description::operator==(const Node &rhs) const {
			const Description &rhs_cast = static_cast<const Description&>(rhs);
			return operator==(rhs_cast);
		}

		bool Description::operator!=(const Description &rhs) const {
			return !(operator==(rhs));
		}

		bool Description::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Description::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Description::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_definitions()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_definitions()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Description::replace matches multiple times (list(Node)::definitions)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Description::replace "
						            << "of children list(Node)::definitions";
					}
				}
				if (new_list->size() != 0) {
					set_definitions(new_list);
				}
				else {
					set_definitions(nullptr);
				}
			}
			return found;
		}

		bool Description::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_definitions()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_definitions()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Description::replace matches multiple times (list(Node)::definitions)";
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
						            << "found an empty node during Description::replace "
						            << "of children list(Node)::definitions";
					}
				}
				if (new_list->size() != 0) {
					set_definitions(new_list);
				}
				else {
					set_definitions(nullptr);
				}
			}
			return found;
		}

		Description::ListPtr Description::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Description>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Description::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_definitions()) {
				for (const Node::Ptr node : *get_definitions()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Description::clone_children(Node::Ptr new_node) const {
			cast_to<Description>(new_node)->set_definitions
				(Node::clone_list(get_definitions()));
		}

		Node::Ptr Description::alloc_same(void) const {
			Ptr p(new Description);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Description &p) {
			os << "Description: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Description::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Description: {nullptr}";
			}

			return os;
		}


		



	}
}