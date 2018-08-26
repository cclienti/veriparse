#include <veriparse/AST/floatconst.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		FloatConst::FloatConst(const std::string &filename, uint32_t line):
			Constant(filename, line)	{
			set_node_type(NodeType::FloatConst);
			set_node_categories({NodeType::Constant, NodeType::Node});
		}

		
		FloatConst::FloatConst(const double &value, const std::string &filename, uint32_t line):
			Constant(filename, line), m_value(value) {
			set_node_type(NodeType::FloatConst);
			set_node_categories({NodeType::Constant, NodeType::Node});
		}
		
		FloatConst &FloatConst::operator=(const FloatConst &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_value(rhs.get_value());
			return *this;
		}

		Node &FloatConst::operator=(const Node &rhs) {
			const FloatConst &rhs_cast = static_cast<const FloatConst&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool FloatConst::operator==(const FloatConst &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_value() != rhs.get_value()) {
				return false;
			}
			return true;
		}

		bool FloatConst::operator==(const Node &rhs) const {
			const FloatConst &rhs_cast = static_cast<const FloatConst&>(rhs);
			return operator==(rhs_cast);
		}

		bool FloatConst::operator!=(const FloatConst &rhs) const {
			return !(operator==(rhs));
		}

		bool FloatConst::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool FloatConst::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool FloatConst::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			return found;
		}

		bool FloatConst::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		FloatConst::ListPtr FloatConst::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<FloatConst>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr FloatConst::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			return list;
		}

		void FloatConst::clone_children(Node::Ptr new_node) const {
		}

		Node::Ptr FloatConst::alloc_same(void) const {
			Ptr p(new FloatConst);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const FloatConst &p) {
			os << "FloatConst: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "value: " << p.get_value();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const FloatConst::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "FloatConst: {nullptr}";
			}

			return os;
		}


		



	}
}