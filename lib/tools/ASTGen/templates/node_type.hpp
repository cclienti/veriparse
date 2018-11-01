#ifndef VERIPARSE_AST_NODE_TYPE_HPP
#define VERIPARSE_AST_NODE_TYPE_HPP

#include <iostream>

namespace Veriparse {
namespace AST {

enum class NodeType {
	{%- for type in nodes_dict.keys() %}
	{{ type }},
	{%- endfor %}
	Node,
	None
};

const char* NodeTypeToString(NodeType node_type);

std::ostream & operator<<(std::ostream &os, const NodeType node_type);

}
}

#endif
