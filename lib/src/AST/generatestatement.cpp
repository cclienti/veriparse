#include <veriparse/AST/generatestatement.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		GenerateStatement::GenerateStatement(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::GenerateStatement);
			set_node_categories({NodeType::Node});
		}

		
		GenerateStatement::GenerateStatement(const Node::ListPtr items, const std::string &filename, uint32_t line):
			Node(filename, line), m_items(items) {
			set_node_type(NodeType::GenerateStatement);
			set_node_categories({NodeType::Node});
		}
		
		GenerateStatement &GenerateStatement::operator=(const GenerateStatement &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &GenerateStatement::operator=(const Node &rhs) {
			const GenerateStatement &rhs_cast = static_cast<const GenerateStatement&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool GenerateStatement::operator==(const GenerateStatement &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool GenerateStatement::operator==(const Node &rhs) const {
			const GenerateStatement &rhs_cast = static_cast<const GenerateStatement&>(rhs);
			return operator==(rhs_cast);
		}

		bool GenerateStatement::operator!=(const GenerateStatement &rhs) const {
			return !(operator==(rhs));
		}

		bool GenerateStatement::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool GenerateStatement::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool GenerateStatement::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_items()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_items()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "GenerateStatement::replace matches multiple times (list(Node)::items)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during GenerateStatement::replace "
						            << "of children list(Node)::items";
					}
				}
				if (new_list->size() != 0) {
					set_items(new_list);
				}
				else {
					set_items(nullptr);
				}
			}
			return found;
		}

		bool GenerateStatement::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_items()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_items()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "GenerateStatement::replace matches multiple times (list(Node)::items)";
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
						            << "found an empty node during GenerateStatement::replace "
						            << "of children list(Node)::items";
					}
				}
				if (new_list->size() != 0) {
					set_items(new_list);
				}
				else {
					set_items(nullptr);
				}
			}
			return found;
		}

		GenerateStatement::ListPtr GenerateStatement::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<GenerateStatement>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr GenerateStatement::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_items()) {
				for (const Node::Ptr node : *get_items()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void GenerateStatement::clone_children(Node::Ptr new_node) const {
			cast_to<GenerateStatement>(new_node)->set_items
				(Node::clone_list(get_items()));
		}

		Node::Ptr GenerateStatement::alloc_same(void) const {
			Ptr p(new GenerateStatement);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const GenerateStatement &p) {
			os << "GenerateStatement: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const GenerateStatement::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "GenerateStatement: {nullptr}";
			}

			return os;
		}


		



	}
}