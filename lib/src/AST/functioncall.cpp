#include <veriparse/AST/functioncall.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		FunctionCall::FunctionCall(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::FunctionCall);
			set_node_categories({NodeType::Node});
		}

		
		FunctionCall::FunctionCall(const Node::ListPtr args, const std::string &name, const std::string &filename, uint32_t line):
			Node(filename, line), m_args(args), m_name(name) {
			set_node_type(NodeType::FunctionCall);
			set_node_categories({NodeType::Node});
		}
		
		FunctionCall &FunctionCall::operator=(const FunctionCall &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_name(rhs.get_name());
			return *this;
		}

		Node &FunctionCall::operator=(const Node &rhs) {
			const FunctionCall &rhs_cast = static_cast<const FunctionCall&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool FunctionCall::operator==(const FunctionCall &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_name() != rhs.get_name()) {
				return false;
			}
			return true;
		}

		bool FunctionCall::operator==(const Node &rhs) const {
			const FunctionCall &rhs_cast = static_cast<const FunctionCall&>(rhs);
			return operator==(rhs_cast);
		}

		bool FunctionCall::operator!=(const FunctionCall &rhs) const {
			return !(operator==(rhs));
		}

		bool FunctionCall::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool FunctionCall::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool FunctionCall::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_args()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_args()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "FunctionCall::replace matches multiple times (list(Node)::args)";
							}
							if(new_node) {
								new_list->push_back(new_node);
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during FunctionCall::replace "
						            << "of children list(Node)::args";
					}
				}
				if (new_list->size() != 0) {
					set_args(new_list);
				}
				else {
					set_args(nullptr);
				}
			}
			return found;
		}

		bool FunctionCall::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			if (get_args()) {
				Node::ListPtr new_list = std::make_shared<Node::List>();
				for (Node::Ptr lnode : *get_args()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "FunctionCall::replace matches multiple times (list(Node)::args)";
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
						            << "found an empty node during FunctionCall::replace "
						            << "of children list(Node)::args";
					}
				}
				if (new_list->size() != 0) {
					set_args(new_list);
				}
				else {
					set_args(nullptr);
				}
			}
			return found;
		}

		FunctionCall::ListPtr FunctionCall::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<FunctionCall>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr FunctionCall::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_args()) {
				for (const Node::Ptr node : *get_args()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			return list;
		}

		void FunctionCall::clone_children(Node::Ptr new_node) const {
			cast_to<FunctionCall>(new_node)->set_args
				(Node::clone_list(get_args()));
		}

		Node::Ptr FunctionCall::alloc_same(void) const {
			Ptr p(new FunctionCall);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const FunctionCall &p) {
			os << "FunctionCall: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "name: " << p.get_name();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const FunctionCall::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "FunctionCall: {nullptr}";
			}

			return os;
		}


		



	}
}