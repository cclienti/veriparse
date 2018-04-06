#include <veriparse/AST/block.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Block::Block(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Block);
			set_node_categories({NodeType::Node});
		}

		
		Block::Block(const Node::ListPtr statements, const std::string &scope, const std::string &filename, uint32_t line):
			Node(filename, line), m_statements(statements), m_scope(scope) {
			set_node_type(NodeType::Block);
			set_node_categories({NodeType::Node});
		}
		
		Block &Block::operator=(const Block &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_scope(rhs.get_scope());
			return *this;
		}

		Node &Block::operator=(const Node &rhs) {
			const Block &rhs_cast = static_cast<const Block&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Block::operator==(const Block &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_scope() != rhs.get_scope()) {
				return false;
			}
			return true;
		}

		bool Block::operator==(const Node &rhs) const {
			const Block &rhs_cast = static_cast<const Block&>(rhs);
			return operator==(rhs_cast);
		}

		bool Block::operator!=(const Block &rhs) const {
			return !(operator==(rhs));
		}

		bool Block::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Block::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Block::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_statements()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_statements()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Block::replace matches multiple times (list(Node)::statements)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during Block::replace "
						            << "of children list(Node)::statements";
					}
				}
				if (new_list->size() != 0) {
					set_statements(new_list);
				}
				else {
					set_statements(nullptr);
				}
			}
			return found;
		}

		bool Block::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_statements()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_statements()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "Block::replace matches multiple times (list(Node)::statements)";
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
						            << "found an empty node during Block::replace "
						            << "of children list(Node)::statements";
					}
				}
				if (new_list->size() != 0) {
					set_statements(new_list);
				}
				else {
					set_statements(nullptr);
				}
			}
			return found;
		}

		Block::ListPtr Block::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Block>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Block::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_statements()) {
				for (const Node::Ptr node : *get_statements()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void Block::clone_children(Node::Ptr new_node) const {
			cast_to<Block>(new_node)->set_statements
				(Node::clone_list(get_statements()));
		}

		Node::Ptr Block::alloc_same(void) const {
			Ptr p(new Block);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Block &p) {
			os << "Block: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "scope: " << p.get_scope();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Block::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Block: {nullptr}";
			}

			return os;
		}


		



	}
}