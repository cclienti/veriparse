#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <iostream>

using namespace Veriparse;


TEST(YAMLImporter, MySource) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.MySource.log");
	Logger::add_stdout_sink();

	std::string str("Source:\n"
	                "  filename:  test.v\n"
	                "  line: 1\n"
	                "  description:\n"
	                "    Description:\n"
	                "      filename: test.v\n"
	                "      line: 2\n"
	                "      definitions:\n"
	                "      - Module:\n"
	                "          filename: test.v\n"
	                "          line: 33\n"
	                "          default_nettype: REG\n"
	                "          name: module0\n"
	                "      - Module:\n"
	                "          filename: test.v\n"
	                "          line: 7\n"
	                "          default_nettype: WIRE\n"
	                "          name: module1\n");

	YAML::Node yaml_ref = YAML::Load(str);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	AST::Source::Ptr ast_cast = AST::cast_to<AST::Source>(ast);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_EQ(yaml["Source"]["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(yaml["Source"]["line"].as<int>(), 1);
	ASSERT_TRUE(yaml["Source"]["description"]["Description"]);
	ASSERT_EQ(yaml["Source"]["description"]["Description"]["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(yaml["Source"]["description"]["Description"]["line"].as<int>(), 2);

	YAML::Node module0 = yaml["Source"]["description"]["Description"]["definitions"][0]["Module"];
	YAML::Node module1 = yaml["Source"]["description"]["Description"]["definitions"][1]["Module"];

	ASSERT_EQ(module0["line"].as<int>(), 33);
	ASSERT_EQ(module0["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(module0["name"].as<std::string>(), "module0");
	ASSERT_EQ(module0["default_nettype"].as<AST::Module::Default_nettypeEnum>(), AST::Module::Default_nettypeEnum::REG);

	ASSERT_EQ(module1["line"].as<int>(), 7);
	ASSERT_EQ(module1["filename"].as<std::string>(), "test.v");
	ASSERT_EQ(module1["name"].as<std::string>(), "module1");
	ASSERT_EQ(module1["default_nettype"].as<AST::Module::Default_nettypeEnum>(), AST::Module::Default_nettypeEnum::WIRE);
}

{% for ntype, ndict in nodes_dict.items() %}

	{%  set inherited_flat = flatten_inherited(ndict['inherited']) %}
	{%- set children_dict_full = merge_dict(ndict['children'], inherited_flat['children']) -%}
	{%- set properties_dict_full = merge_dict(ndict['properties'], inherited_flat['properties']) -%}
	{%- set properties_user_enum_full = merge_dict(ndict['properties_user_enum'], inherited_flat['properties_user_enum']) %}

TEST(YAMLImporter, {{ ntype }}) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("YAMLImporter.{{ ntype }}.log");
	Logger::add_stdout_sink();

	{%- set seed = random_seed(0) %}
	std::string str (
		"{{ ntype }}:\n"
		"  filename: {{ ntype.lower() }}.v\n"
		"  line: {{ ntype | length }}\n"
		{%- for cname in children_dict_full.keys() %}
		"  {{ cname }}:\n"
		{%- endfor %}
		{%- for pname, ptype in properties_dict_full.items() %}
		{%- if ptype in ndict['properties_user_enum'] %}
		"  {{ pname }}: {{ remove_namespace(get_random_property_value(pname, ndict['properties'], ndict['properties_user_enum'])) }}\n"
		{%- elif ptype in inherited_flat['properties_user_enum'] %}
		"  {{ pname }}: {{ remove_namespace(remove_namespace(get_random_property_value(pname, inherited_flat['properties'], inherited_flat['properties_user_enum']))) }}\n"
		{%- else %}
		"  {{ pname }}: {{ remove_escaped_quote(get_random_property_value(pname, properties_dict_full, properties_user_enum_full)) }}\n"
		{%- endif %}
		{%- endfor %}
	);

	AST::Node::Ptr ast = Importers::YAMLImporter().import(str);
	YAML::Node yaml = Generators::YAMLGenerator().render(ast);

	ASSERT_TRUE(yaml["{{ ntype }}"]);
	ASSERT_TRUE(yaml["{{ ntype }}"]["filename"].as<std::string>() == "{{ ntype.lower() }}.v");
	ASSERT_TRUE(yaml["{{ ntype }}"]["line"].as<int>() == {{ ntype | length }});

	{%- for cname, ctype in children_dict_full.items() %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ cname }}"]);
	{%- endfor %}

	{%- set seed = random_seed(0) -%}
	{%- for pname, ptype in properties_dict_full.items() %}
	{%- if ptype in ndict['properties_user_enum'] %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ pname }}"].as<AST::{{ ntype }}::{{ ptype }}>() == AST::{{ ntype }}::{{ get_random_property_value(pname, ndict['properties'], ndict['properties_user_enum']) }});
	{%- elif ptype in inherited_flat['properties_user_enum'] %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ pname }}"].as<AST::{{ ptype }}>() == AST::{{ get_random_property_value(pname, inherited_flat['properties'], inherited_flat['properties_user_enum']) }});
	{%- else %}
	ASSERT_TRUE(yaml["{{ ntype }}"]["{{ pname }}"].as<{{ ptype }}>() == {{ get_random_property_value(pname, properties_dict_full, properties_user_enum_full) }});
	{%- endif %}
	{%- endfor %}
}
{% endfor %}
