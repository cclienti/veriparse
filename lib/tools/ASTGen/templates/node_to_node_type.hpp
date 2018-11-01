#ifndef VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP
#define VERIPARSE_AST_NODE_TO_NODE_TYPE_HPP

#include <veriparse/AST/nodes.hpp>
#include <iostream>

namespace Veriparse {
namespace AST {

template<typename NodeT> constexpr NodeType get_node_type() {
	return NodeType::None;
}

template<> constexpr NodeType get_node_type<Node>() {return NodeType::Node;}
{%- for type in nodes_dict.keys() %}
template<> constexpr NodeType get_node_type<{{ type }}>() {return NodeType::{{ type }};}
{%- endfor %}

}
}

#endif
