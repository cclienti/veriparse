#include <veriparse/AST/partselectplusindexed.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		PartselectPlusIndexed::PartselectPlusIndexed(const std::string &filename, uint32_t line):
			PartselectIndexed(filename, line)	{
			set_node_type(NodeType::PartselectPlusIndexed);
			set_node_categories({NodeType::PartselectIndexed, NodeType::Indirect, NodeType::Node});
		}

		
		PartselectPlusIndexed::PartselectPlusIndexed(const Node::Ptr index, const Node::Ptr size, const Node::Ptr var, const std::string &filename, uint32_t line):
			PartselectIndexed(index, size, var, filename, line) {
			set_node_type(NodeType::PartselectPlusIndexed);
			set_node_categories({NodeType::PartselectIndexed, NodeType::Indirect, NodeType::Node});
		}
		
		PartselectPlusIndexed &PartselectPlusIndexed::operator=(const PartselectPlusIndexed &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &PartselectPlusIndexed::operator=(const Node &rhs) {
			const PartselectPlusIndexed &rhs_cast = static_cast<const PartselectPlusIndexed&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool PartselectPlusIndexed::operator==(const PartselectPlusIndexed &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool PartselectPlusIndexed::operator==(const Node &rhs) const {
			const PartselectPlusIndexed &rhs_cast = static_cast<const PartselectPlusIndexed&>(rhs);
			return operator==(rhs_cast);
		}

		bool PartselectPlusIndexed::operator!=(const PartselectPlusIndexed &rhs) const {
			return !(operator==(rhs));
		}

		bool PartselectPlusIndexed::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool PartselectPlusIndexed::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool PartselectPlusIndexed::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_index()) {
				if (get_index() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectPlusIndexed::replace matches multiple times (Node::index)";
					}
					set_index(new_node);
					found = true;
				}
			}
			if (get_size()) {
				if (get_size() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectPlusIndexed::replace matches multiple times (Node::size)";
					}
					set_size(new_node);
					found = true;
				}
			}
			if (get_var()) {
				if (get_var() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "PartselectPlusIndexed::replace matches multiple times (Node::var)";
					}
					set_var(new_node);
					found = true;
				}
			}
			return found;
		}

		bool PartselectPlusIndexed::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		PartselectPlusIndexed::ListPtr PartselectPlusIndexed::clone_list(const ListPtr nodes) {
			ListPtr list = std::make_shared<List>();
			if (nodes) {
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<PartselectPlusIndexed>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr PartselectPlusIndexed::get_children(void) const {
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

		void PartselectPlusIndexed::clone_children(Node::Ptr new_node) const {
			if (get_index()) {
				cast_to<PartselectPlusIndexed>(new_node)->set_index
					(get_index()->clone());
			}
			if (get_size()) {
				cast_to<PartselectPlusIndexed>(new_node)->set_size
					(get_size()->clone());
			}
			if (get_var()) {
				cast_to<PartselectPlusIndexed>(new_node)->set_var
					(get_var()->clone());
			}
		}

		Node::Ptr PartselectPlusIndexed::alloc_same(void) const {
			Ptr p(new PartselectPlusIndexed);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const PartselectPlusIndexed &p) {
			os << "PartselectPlusIndexed: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const PartselectPlusIndexed::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "PartselectPlusIndexed: {nullptr}";
			}

			return os;
		}


		



	}
}