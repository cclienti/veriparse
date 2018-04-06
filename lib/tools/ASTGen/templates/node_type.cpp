#include <veriparse/AST/node_type.hpp>
#include <vector>


namespace Veriparse {
	namespace AST {

		const char* NodeTypeToString(NodeType node_type) {
			switch (node_type) {
			case NodeType::Node: return "Node";
			{%- for type in nodes_dict.keys() %}
			case NodeType::{{ type }}: return "{{ type }}";
			{%- endfor %}
			default: return "Unknown NodeType";
			}
		}

		std::ostream & operator<<(std::ostream &os, const NodeType node_type)
		{
			os << NodeTypeToString(node_type);
			return os;
		}

		std::ostream & operator<<(std::ostream &os, const std::vector<NodeType> &node_categories)
		{
			os << "[";
			for(const NodeType &cat: node_categories) {
				os << NodeTypeToString(cat);
				if(cat != node_categories.back())
					os << ", ";
			}
			os << "]";
			return os;
		}

	}
}
