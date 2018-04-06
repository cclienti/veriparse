#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

using namespace Veriparse;

{% for ntype, ndict in nodes_dict.items() %}
TEST(YAMLGenerator, {{ ntype }}) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLGenerator.{{ ntype }}.log");
	Logger::add_stdout_sink();

	{%  set inherited_flat = flatten_inherited(ndict['inherited']) %}
	{%- set children_dict_full = merge_dict(ndict['children'], inherited_flat['children']) -%}
	{%- set properties_dict_full = merge_dict(ndict['properties'], inherited_flat['properties']) -%}
	{%- set properties_user_enum_full = merge_dict(ndict['properties_user_enum'], inherited_flat['properties_user_enum']) %}

	{%- for cname, ctype in children_dict_full.items() %}
	{%- if is_type_list(ctype) %}
	AST::{{ get_type(ctype) }} c_{{ cname }}(new AST::{{ get_underlying_type(ctype) }}::List);
	{%- else %}
	AST::{{ get_type(ctype) }} c_{{ cname }}(new AST::{{ get_underlying_type(ctype) }});
	{%- endif %}
	{%- endfor %}

	{%- set seed = random_seed(0) -%}

	{%- for pname, ptype in properties_dict_full.items() %}
	{%- if ptype in properties_user_enum_full %}
	AST::{{ ntype }}::{{ ptype }} p_{{ pname }} = AST::{{ ntype }}::{{ get_random_property_value(pname, properties_dict_full, properties_user_enum_full) }};
	{%- else %}
	{{ ptype }} p_{{ pname }} = {{ get_random_property_value(pname, properties_dict_full, properties_user_enum_full) }};
	{%- endif %}
	{%- endfor %}

	{%- set c_args = prepend_list_element("c_", children_dict_full.keys()) %}
	{%- set p_args = prepend_list_element("p_", properties_dict_full.keys()) %}
	{%- set args = list_concat(c_args, p_args) %}

	AST::{{ get_type(ntype) }} m_{{ get_underlying_type(ntype).lower() }}(new AST::{{ get_underlying_type(ntype) }}
		({%- for a in args %}{{ a }}, {% endfor -%} "filename", 0));

	YAML::Node yaml = Generators::YAMLGenerator().render(m_{{ get_underlying_type(ntype).lower() }});

	ASSERT_TRUE(yaml["{{ ntype }}"]);

	{%- for cname, ctype in children_dict_full.items() %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ cname }}"]);
	{%- endfor %}

	{%- set seed = random_seed(0) -%}
	{%- for pname, ptype in properties_dict_full.items() %}
	{%- if ptype in properties_user_enum_full %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ pname }}"].as<AST::{{ ntype }}::{{ ptype }}>() == AST::{{ ntype }}::{{ get_random_property_value(pname, properties_dict_full, properties_user_enum_full) }});
	{%- else %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ pname }}"].as<{{ ptype }}>() == {{ get_random_property_value(pname, properties_dict_full, properties_user_enum_full) }});
	{%- endif %}
	{%- endfor %}
}
{% endfor %}
