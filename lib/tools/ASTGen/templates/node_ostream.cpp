#include <veriparse/AST/node_ostream.hpp>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse {
	namespace AST {

		std::ostream & operator<<(std::ostream &os, const Node &p)
		{
			switch (p.get_node_type()) {
			{%- for type in nodes_dict.keys() %}
			case NodeType::{{ type }}: os << static_cast<const AST::{{ get_underlying_type(type) }} &>(p); break;
			{%- endfor %}
			default: os << "Node: {}";
			}

			return os;
		}

		std::ostream & operator<<(std::ostream &os, const Node::Ptr p)
		{
			if (p) {
				os << *p;
			}
			else {
				os << "Node: {nullptr}";
			}

			return os;
		}


	}
}
