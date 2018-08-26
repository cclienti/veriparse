#include <veriparse/AST/constant.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Constant::Constant(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Constant);
			set_node_categories({NodeType::Node});
		}

		
		Constant &Constant::operator=(const Constant &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			return *this;
		}

		Node &Constant::operator=(const Node &rhs) {
			const Constant &rhs_cast = static_cast<const Constant&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Constant::operator==(const Constant &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			return true;
		}

		bool Constant::operator==(const Node &rhs) const {
			const Constant &rhs_cast = static_cast<const Constant&>(rhs);
			return operator==(rhs_cast);
		}

		bool Constant::operator!=(const Constant &rhs) const {
			return !(operator==(rhs));
		}

		bool Constant::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Constant::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Constant::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			return found;
		}

		bool Constant::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Constant::ListPtr Constant::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Constant>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Constant::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			return list;
		}

		void Constant::clone_children(Node::Ptr new_node) const {
		}

		Node::Ptr Constant::alloc_same(void) const {
			Ptr p(new Constant);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Constant &p) {
			os << "Constant: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Constant::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Constant: {nullptr}";
			}

			return os;
		}


		



	}
}