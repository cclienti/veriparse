#include <veriparse/AST/{{ class_name.lower() }}.hpp>
#include <veriparse/AST/node_cast.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

{% set children_dict_inherited = flatten_inherited(inherited)['children'] -%}

{%- set children_dict_full = merge_dict(children_dict, children_dict_inherited) -%}

{%- set properties_dict_inherited = flatten_inherited(inherited)['properties'] -%}

{%- set properties_dict_full = merge_dict(properties_dict, properties_dict_inherited) -%}

{%- macro children_ctor_params() -%}
{% for cname, ctype in children_dict_full.items() %}const {{ get_type(ctype) }} {{ cname }}, {% endfor %}
{%- endmacro -%}

{% macro properties_ctor_params() -%}
{% for name, type in properties_dict_full.items() %}const {{ type }} &{{ name }}, {% endfor %}
{%- endmacro -%}

{%- macro children_ctor_base_args() -%}
{% for cname, ctype in children_dict_inherited.items() %}{{ cname }}, {% endfor %}
{%- endmacro -%}

{% macro properties_ctor_base_args() -%}
{% for name, type in properties_dict_inherited.items() %}{{ name }}, {% endfor %}
{%- endmacro -%}

{%- macro ctor_init(d) -%}
{%- for dname, _ in d.items() -%}, m_{{ dname }}({{ dname }}){%- endfor -%}
{%- endmacro %}

{%- macro inherited_list(list) -%}
{%- for inherited in list -%}NodeType::{{ inherited }}{% if not loop.last %}, {% endif %}{%- endfor -%}
{%- endmacro %}

{%- macro category_init_list() -%}
{ {%- for base_class in inherited -%}NodeType::{{ base_class}}, {% endfor -%} NodeType::Node}
{%- endmacro -%}

namespace Veriparse {
	namespace AST {

		{{ class_name }}::{{ class_name }}(const std::string &filename, uint32_t line):
			{{ base_class }}(filename, line)	{
			set_node_type(NodeType::{{ class_name }});
			set_node_categories({{ category_init_list() }});
		}

		{% if (properties_dict_full|length > 0) or (children_dict_full|length > 0) %}
		{{ class_name }}::{{ class_name }}({{ children_ctor_params() }}{{ properties_ctor_params() }}const std::string &filename, uint32_t line):
			{{ base_class }}({{ children_ctor_base_args() }}{{ properties_ctor_base_args() }}filename, line){{ ctor_init(children_dict) }}{{ ctor_init(properties_dict) }} {
			set_node_type(NodeType::{{ class_name }});
			set_node_categories({{ category_init_list() }});
		}
		{% endif %}
		{{ class_name }} &{{ class_name }}::operator=(const {{ class_name }} &rhs) {
			Node::operator=(static_cast<const Node &>(rhs));
			{%- for prop_name, _ in properties_dict_full.items() %}
			set_{{ prop_name }}(rhs.get_{{ prop_name }}());
			{%- endfor %}
			return *this;
		}

		Node &{{ class_name }}::operator=(const Node &rhs) {
			const {{ class_name }} &rhs_cast = static_cast<const {{ class_name }}&>(rhs);
			return static_cast<Node &> (operator=(rhs_cast));
		}

		bool {{ class_name }}::operator==(const {{ class_name }} &rhs) const {
			if (Node::operator==(rhs) == false) {
				return false;
			}

			{%- for prop_name, _ in properties_dict_full.items() %}
			if (get_{{ prop_name }}() != rhs.get_{{ prop_name }}()) {
				return false;
			}
			{%- endfor %}
			return true;
		}

		bool {{ class_name }}::operator==(const Node &rhs) const {
			const {{ class_name }} &rhs_cast = static_cast<const {{ class_name }}&>(rhs);
			return operator==(rhs_cast);
		}

		bool {{ class_name }}::operator!=(const {{ class_name }} &rhs) const {
			return !(operator==(rhs));
		}

		bool {{ class_name }}::operator!=(const Node &rhs) const {
			return !(operator==(rhs));
		}

		bool {{ class_name }}::remove(Node::Ptr node) {
			return replace(node, AST::Node::Ptr(nullptr));
		}

		bool {{ class_name }}::replace(Node::Ptr node, Node::Ptr new_node) {
			bool found = false;
			{%- for cname, ctype in children_dict_full.items() %}
			{%- if is_type_list(ctype) %}
			if (get_{{ cname }}()) {
				{{ get_type(ctype) }} new_list = std::make_shared<{{ get_underlying_type(ctype) }}::List>();
				for ({{ get_type(get_underlying_type(ctype)) }} lnode : *get_{{ cname }}()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "{{ class_name }}::replace matches multiple times ({{ ctype }}::{{ cname }})";
							}
							if(new_node) {
								{%- if get_underlying_type(ctype) == "Node" %}
								new_list->push_back(new_node);
								{%- else %}
								new_list->push_back(cast_to<{{ get_underlying_type(ctype) }}>(new_node));
								{%- endif %}
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during {{ class_name }}::replace "
						            << "of children {{ ctype }}::{{ cname }}";
					}
				}
				if (new_list->size() != 0) {
					set_{{ cname }}(new_list);
				}
				else {
					set_{{ cname }}(nullptr);
				}
			}
			{%- else %}
			if (get_{{ cname }}()) {
				if (get_{{ cname }}() == node) {
					if (found) {
						LOG_WARNING << *this << ", "
						            << "{{ class_name }}::replace matches multiple times ({{ ctype }}::{{ cname }})";
					}
					{%- if get_underlying_type(ctype) == "Node" %}
					set_{{ cname }}(new_node);
					{%- else %}
					set_{{ cname }}(cast_to<{{ get_underlying_type(ctype) }}>(new_node));
					{%- endif %}
					found = true;
				}
			}
			{%- endif %}
			{%- endfor %}
			return found;
		}

		bool {{ class_name }}::replace(Node::Ptr node, Node::ListPtr new_nodes) {
			bool found = false;
			{%- for cname, ctype in children_dict_full.items() %}
			{%- if is_type_list(ctype) %}
			if (get_{{ cname }}()) {
				{{ get_type(ctype) }} new_list = std::make_shared<{{ get_underlying_type(ctype) }}::List>();
				for ({{ get_type(get_underlying_type(ctype)) }} lnode : *get_{{ cname }}()) {
					if (lnode) {
						if (lnode != node) {
							new_list->push_back(lnode);
						}
						else {
							if (found) {
								LOG_WARNING << *this << ", "
								            << "{{ class_name }}::replace matches multiple times ({{ ctype }}::{{ cname }})";
							}
							if(new_nodes) {
								{%- if get_underlying_type(ctype) == "Node" %}
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(n);
								{%- else %}
								for(Node::Ptr n: *new_nodes)
									new_list->push_back(cast_to<{{ get_underlying_type(ctype) }}>(n));
								{%- endif %}
							}
							found = true;
						}
					}
					else {
						LOG_WARNING << *this << ", "
						            << "found an empty node during {{ class_name }}::replace "
						            << "of children {{ ctype }}::{{ cname }}";
					}
				}
				if (new_list->size() != 0) {
					set_{{ cname }}(new_list);
				}
				else {
					set_{{ cname }}(nullptr);
				}
			}
			{%- endif %}
			{%- endfor %}
			return found;
		}

		{{ class_name }}::ListPtr {{ class_name }}::clone_list(const ListPtr nodes) {
			ListPtr list;
			if (nodes) {
                list = std::make_shared<List>();
				for(const Ptr p : *nodes) {
					list->push_back(cast_to<{{ class_name }}>(p->clone()));
				}
			}
			return list;
		}

		Node::ListPtr {{ class_name }}::get_children(void) const {
			Node::ListPtr list = std::make_shared<Node::List>();
			{%- for cname, ctype in children_dict_full.items() %}
			{%- if is_type_list(ctype) %}
			if (get_{{ cname }}()) {
				for (const {{ get_type(get_underlying_type(ctype)) }} node : *get_{{ cname }}()) {
					if (node) {
						list->push_back(std::static_pointer_cast<Node>(node));
					}
				}
			}
			{%- else %}
			if (get_{{ cname }}()) {
				list->push_back(std::static_pointer_cast<Node>(get_{{ cname }}()));
			}
			{%- endif %}
			{%- endfor %}
			return list;
		}

		void {{ class_name }}::clone_children(Node::Ptr new_node) const {
			{%- for cname, ctype in children_dict_full.items() %}
			{%- if is_type_list(ctype) %}
			cast_to<{{ class_name }}>(new_node)->set_{{ cname }}
				({{ get_underlying_type(ctype) }}::clone_list(get_{{ cname }}()));
			{%- else %}
			if (get_{{ cname }}()) {
				{%- if get_underlying_type(ctype) == "Node" %}
				cast_to<{{ class_name }}>(new_node)->set_{{ cname }}
					(get_{{ cname }}()->clone());
				{%- else %}
				cast_to<{{ class_name }}>(new_node)->set_{{ cname }}
					(cast_to<{{ get_underlying_type(ctype) }}>(get_{{ cname }}()->clone()));
				{%- endif %}
			}
			{%- endif %}
			{%- endfor %}
		}

		Node::Ptr {{ class_name }}::alloc_same(void) const {
			Ptr p(new {{ class_name }});
			return p;
		}


		std::ostream & operator<<(std::ostream &os, const {{ class_name }} &p) {
			os << "{{ class_name }}: {";
			if (!p.get_filename().empty()) {
				os << "filename: " << p.get_filename() << ", "
				   << "line: " << p.get_line();
			}
			{%- for pname, ptype in properties_dict_full.items() %}
			{% if loop.first %}
			if (!p.get_filename().empty()) os << ", ";
			{% endif %}
			os << "{{ pname }}: " << p.get_{{ pname }}(){% if not loop.last %}<< ", "{% endif %};
			{%- endfor %}
			os << "}";
			return os;
		}


		std::ostream & operator<<(std::ostream &os, const {{ class_name }}::Ptr p) {
			if (p) {
				os << *p;
			}
			else {
				os << "{{ class_name }}: {nullptr}";
			}

			return os;
		}


		{% for penum_name, penum_list in properties_user_enum.items() %}
		std::ostream & operator<<(std::ostream &os, const {{ class_name }}::{{ penum_name }} p) {
			switch(p) {
			{%- for pval in penum_list %}
			case {{ class_name }}::{{ penum_name }}::{{ pval }}: os << "{{ pval }}"; break;
			{%- endfor %}
			default: break;
			}
			return os;
		}

		{% endfor %}



	}
}
