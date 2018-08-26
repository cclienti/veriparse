#include <veriparse/AST/sens.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

namespace Veriparse {
	namespace AST {

		Sens::Sens(const std::string &filename, uint32_t line):
			Node(filename, line)	{
			set_node_type(NodeType::Sens);
			set_node_categories({NodeType::Node});
		}

		
		Sens::Sens(const Node::Ptr sig, const TypeEnum &type, const std::string &filename, uint32_t line):
			Node(filename, line), m_sig(sig), m_type(type) {
			set_node_type(NodeType::Sens);
			set_node_categories({NodeType::Node});
		}
		
		Sens &Sens::operator=(const Sens &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			set_type(rhs.get_type());
			return *this;
		}

		Node &Sens::operator=(const Node &rhs) {
			const Sens &rhs_cast = static_cast<const Sens&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool Sens::operator==(const Sens &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}
			if (get_type() != rhs.get_type()) {
				return false;
			}
			return true;
		}

		bool Sens::operator==(const Node &rhs) const {
			const Sens &rhs_cast = static_cast<const Sens&>(rhs);
			return operator==(rhs_cast);
		}

		bool Sens::operator!=(const Sens &rhs) const {
			return !(operator==(rhs));
		}

		bool Sens::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool Sens::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool Sens::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			if (get_sig()) {
				if (get_sig() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "Sens::replace matches multiple times (Node::sig)";
					}
					set_sig(new_node);
					found = true;
				}
			}
			return found;
		}

		bool Sens::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			return found;
		}

		Sens::ListPtr Sens::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<Sens>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr Sens::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			if (get_sig()) {
				list->push_back(std::static_pointer_cast<Node>(get_sig()));
			}
			return list;
		}

		void Sens::clone_children(Node::Ptr new_node) const {
			if (get_sig()) {
				cast_to<Sens>(new_node)->set_sig
					(get_sig()->clone());
			}
		}

		Node::Ptr Sens::alloc_same(void) const {
			Ptr p(new Sens);
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const Sens &p) {
			os << "Sens: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			
			if (!p.get_filename().empty()) os << ", ";
			
			os << "type: " << p.get_type();
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const Sens::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "Sens: {nullptr}";
			}

			return os;
		}


		
		std::ostream & operator<<(std::ostream &os, const Sens::TypeEnum p) {
			switch(p) {
			case Sens::TypeEnum::ALL: os << "ALL"; break;
			case Sens::TypeEnum::POSEDGE: os << "POSEDGE"; break;
			case Sens::TypeEnum::NEGEDGE: os << "NEGEDGE"; break;
			case Sens::TypeEnum::NONE: os << "NONE"; break;
			default: break;
			}
			return os;
		}

		



	}
}