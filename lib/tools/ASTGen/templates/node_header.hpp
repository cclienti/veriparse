#ifndef VERIPARSE_AST_{{ class_name.upper() }}_HPP
#define VERIPARSE_AST_{{ class_name.upper() }}_HPP

{% set children_dict_full =
		merge_dict(children_dict, flatten_inherited(inherited)['children']) -%}

{%- set properties_dict_full =
		merge_dict(properties_dict, flatten_inherited(inherited)['properties']) -%}

{%- set children_underlying_types_full =
		merge_set(children_underlying_types, flatten_inherited(inherited)['children_underlying_types']) -%}

{%- macro children_ctor_params() -%}
{% for cname, ctype in children_dict_full.items() %}const {{ get_type(ctype) }} {{ cname }}, {% endfor %}
{%- endmacro -%}

{%- macro properties_ctor_params() -%}
{% for name, type in properties_dict_full.items() %}const {{ type }} &{{ name }}, {% endfor %}
{%- endmacro -%}

#include <veriparse/AST/node.hpp>
{%- if base_class.lower() != "node" %}
#include <veriparse/AST/{{ base_class.lower() }}.hpp>
{% endif -%}

{%- for header_name in children_underlying_types_full|sort %}
{%- if header_name.lower() not in ["node", base_class.lower()] %}
#include <veriparse/AST/{{ header_name.lower() }}.hpp>
{%- endif %}
{%- endfor %}

{%- for header in includes|sort %}
#include <{{ header }}>
{%- endfor %}

#include <list>
#include <string>
#include <cstdint>
#include <memory>
#include <iostream>
#include <type_traits>

namespace Veriparse {
namespace AST {

class {{ class_name }}: public {{ base_class }} {
public:
	using Ptr = typename NodePointers<{{ class_name }}>::Ptr;
	using List = typename NodePointers<{{ class_name }}>::List;
	using ListPtr = typename NodePointers<{{ class_name }}>::ListPtr;

	{% for penum_name, penum_list in properties_user_enum.items() -%}
	enum class {{ penum_name }} {
		{%- for pval in penum_list %}
		{{ pval }}{% if not loop.last %}, {% endif %}
		{%- endfor %}
	};

	{% endfor -%}

	/**
	 * Constructor, m_node_type is set to NodeType::{{ class_name }}.
	 */
	{{ class_name }}(const std::string &filename="", uint32_t line=0);
	{% if (properties_dict_full|length > 0) or (children_dict_full|length > 0) %}
	/**
	 * Constructor, m_node_type is set to NodeType::{{ class_name }}.
	 */
	{{ class_name }}({{ children_ctor_params() }}{{ properties_ctor_params() }}const std::string &filename="", uint32_t line=0);
	{% endif %}

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual {{ class_name }} &operator=(const {{ class_name }} &rhs);

	/**
	 * Assignment operator, do not affect children.
	 */
	virtual Node &operator=(const Node &rhs) override;

	/**
	 * Return true if the {{ class_name }} nodes are the same, do not check children.
	 */
	virtual bool operator==(const {{ class_name }} &rhs) const;

	/**
	 * Return true if the {{ class_name }} nodes are the same, do not check children.
	 */
	virtual bool operator==(const Node &rhs) const override;

	/**
	 * Return true if the {{ class_name }} nodes are the same, do not check children.
	 */
	virtual bool operator!=(const {{ class_name }} &rhs) const;

	/**
	 * Return true if the {{ class_name }} nodes are the same, do not check children.
	 */
	virtual bool operator!=(const Node &rhs) const override;

	/**
	 * Remove the given child.
	 */
	virtual bool remove(Node::Ptr node) override;

	/**
	 * Replace the given children node by the new node.
	 */
	virtual bool replace(Node::Ptr node, Node::Ptr new_node) override;

	/**
	 * Replace the given children node by the new nodes in the list.
	 */
	virtual bool replace(Node::Ptr node, Node::ListPtr new_nodes) override;
	{% for cname, ctype in children_dict.items() %}
	/**
	 * Return the child {{ cname }}.
	 */
	virtual {{ get_type(ctype) }} get_{{ cname }}(void) const {return m_{{ cname }};}
	{% endfor -%}

	{% for cname, ctype in children_dict.items() %}
	/**
	 * Change the child {{ cname }}.
	 */
	virtual void set_{{ cname }}({{ get_type(ctype) }} {{ cname }}) {
		m_{{ cname }} = {{ cname }};
	}
	{% endfor -%}

	{% for pname, ptype in properties_dict.items() %}
	/**
	 * Return the property {{ pname }}.
	 */
	virtual const {{ ptype }} &get_{{ pname }}(void) const {return m_{{ pname }};}
	{% endfor -%}

	{% for pname, ptype in properties_dict.items() %}
	/**
	 * Change the property {{ pname }}.
	 */
	virtual void set_{{ pname }}(const {{ ptype }} &{{ pname }}) {m_{{ pname }} = {{ pname }};}
	{% endfor %}

	/**
	 * Return the children list using the private children member
	 * pointers.
	 */
	virtual Node::ListPtr get_children(void) const override;

	/**
	 * Return a clone of the current List and of all children (recursive).
	 */
	static ListPtr clone_list(const ListPtr nodes);

protected:
	/**
	 * Clone the instance children and attach them to the
	 * new_node. This method is called by Node::clone().
	 */
	virtual void clone_children(Node::Ptr new_node) const override;

private:
	/**
	 * Allocate a new node with the same node type than the current instance.
	 */
	virtual Node::Ptr alloc_same(void) const override;

	{% for child_name, child_type in children_dict.items() %}
	{{ get_type(child_type) }} m_{{ child_name }};
	{%- endfor %}
	{%- for prop_name, prop_type in properties_dict.items() %}
	{{ prop_type }} m_{{ prop_name }};
	{%- endfor %}
};

std::ostream & operator<<(std::ostream &os, const {{ class_name }} &p);
std::ostream & operator<<(std::ostream &os, const {{ class_name }}::Ptr p);
{% for penum_name, penum_list in properties_user_enum.items() %}
std::ostream & operator<<(std::ostream &os, const {{ class_name }}::{{ penum_name }} p);
{% endfor %}


}
}


#endif
