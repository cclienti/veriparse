#include <veriparse/AST/partselectindexed.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		PartselectIndexed::PartselectIndexed(const std::string &filename, uint32_t line):
			Indirect(filename, line)	{
			set_node_type(NodeType::PartselectIndexed);
			set_node_categories({NodeType::Indirect, NodeType::Node});
		}

		
		PartselectIndexed::PartselectIndexed(const Node::Ptr index, const Node::Ptr size, const Node::Ptr var, const std::string &filename, uint32_t line):
			Indirect(var, filename, line), m_index(index), m_size(size) {
			set_node_type(NodeType::PartselectIndexed);
			set_node_categories({NodeType::Indirect, NodeType::Node});
		}
		
		PartselectIndexed &PartselectIndexed::operator=(const PartselectIndexed &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &PartselectIndexed::operator=(const Node &rhs) {
			const PartselectIndexed &rhs_cast = static_cast<const PartselectIndexed&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool PartselectIndexed::operator==(const PartselectIndexed &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool PartselectIndexed::operator==(const Node &rhs) const {
			const PartselectIndexed &rhs_cast = static_cast<const PartselectIndexed&>(rhs);
			return operator==(rhs_cast);
		}

		bool PartselectIndexed::operator!=(const PartselectIndexed &rhs) const {
			return !(operator==(rhs));
		}

		bool PartselectIndexed::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool PartselectIndexed::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool PartselectIndexed::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_index()) {
				if (get_index() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectIndexed::replace matches multiple times (Node::index)";
					}
					set_index(new_node);
					found = true;
				}
			}
			if (get_size()) {
				if (get_size() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectIndexed::replace matches multiple times (Node::size)";
					}
					set_size(new_node);
					found = true;
				}
			}
			if (get_var()) {
				if (get_var() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectIndexed::replace matches multiple times (Node::var)";
					}
					set_var(new_node);
					found = true;
				}
			}
			return found;
		}

		bool PartselectIndexed::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		PartselectIndexed::ListPtr PartselectIndexed::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<PartselectIndexed>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr PartselectIndexed::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_index()) {
				list->push_back(std::static_pointer_cast<Node>(get_index()));
			}
			if (get_size()) {
				list->push_back(std::static_pointer_cast<Node>(get_size()));
			}
			if (get_var()) {
				list->push_back(std::static_pointer_cast<Node>(get_var()));
			}
			return list;
		}

		void PartselectIndexed::clone_children(Node::Ptr new_node) const {
			if (get_index()) {
				cast_to<PartselectIndexed>(new_node)->set_index
					(get_index()->clone());
			}
			if (get_size()) {
				cast_to<PartselectIndexed>(new_node)->set_size
					(get_size()->clone());
			}
			if (get_var()) {
				cast_to<PartselectIndexed>(new_node)->set_var
					(get_var()->clone());
			}
		}

		Node::Ptr PartselectIndexed::alloc_same(void) const {
			Ptr p(new PartselectIndexed);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const PartselectIndexed &p) {
			os << "PartselectIndexed: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const PartselectIndexed::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "PartselectIndexed: {nullptr}";
			}

			return os;
		}


		



	}
}