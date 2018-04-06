#include <veriparse/AST/parallelblock.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		ParallelBlock::ParallelBlock(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::ParallelBlock);
			set_node_categories({NodeType::Node});
		}

		
		ParallelBlock::ParallelBlock(const Node::ListPtr statements, const std::string &scope, const std::string &filename, uint32_t line):
			Node(filename, line), m_statements(statements), m_scope(scope) {
			set_node_type(NodeType::ParallelBlock);
			set_node_categories({NodeType::Node});
		}
		
		ParallelBlock &ParallelBlock::operator=(const ParallelBlock &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_scope(rhs.get_scope());
			return *this;
		}

		Node &ParallelBlock::operator=(const Node &rhs) {
			const ParallelBlock &rhs_cast = static_cast<const ParallelBlock&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool ParallelBlock::operator==(const ParallelBlock &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_scope() != rhs.get_scope()) {
				return false;
			}
			return true;
		}

		bool ParallelBlock::operator==(const Node &rhs) const {
			const ParallelBlock &rhs_cast = static_cast<const ParallelBlock&>(rhs);
			return operator==(rhs_cast);
		}

		bool ParallelBlock::operator!=(const ParallelBlock &rhs) const {
			return !(operator==(rhs));
		}

		bool ParallelBlock::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool ParallelBlock::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool ParallelBlock::replace(Node::Ptr node, Node::Ptr new_node) {
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
								            << "ParallelBlock::replace matches multiple times (list(Node)::statements)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during ParallelBlock::replace "
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

		bool ParallelBlock::replace(Node::Ptr node, Node::ListPtr new_nodes) {
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
								            << "ParallelBlock::replace matches multiple times (list(Node)::statements)";
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
						            << "found an empty node during ParallelBlock::replace "
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

		ParallelBlock::ListPtr ParallelBlock::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<ParallelBlock>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr ParallelBlock::get_children(void) const {
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

		void ParallelBlock::clone_children(Node::Ptr new_node) const {
			cast_to<ParallelBlock>(new_node)->set_statements
				(Node::clone_list(get_statements()));
		}

		Node::Ptr ParallelBlock::alloc_same(void) const {
			Ptr p(new ParallelBlock);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const ParallelBlock &p) {
			os << "ParallelBlock: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "scope: " << p.get_scope();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const ParallelBlock::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "ParallelBlock: {nullptr}";
			}

			return os;
		}


		



	}
}