#ifndef VERIPARSE_AST_NODES_HPP
#define VERIPARSE_AST_NODES_HPP

#include <veriparse/AST/node.hpp>
{%- for header_name in header_list %}
#include <veriparse/AST/{{ header_name.lower() }}.hpp>
{%- endfor %}
#include <veriparse/AST/node_operators.hpp>
#include <veriparse/AST/node_cast.hpp>

#endif
