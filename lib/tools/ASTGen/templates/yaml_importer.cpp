#include <veriparse/importers/yaml_importer.hpp>
#include <veriparse/importers/yaml_specializations.hpp>
#include <veriparse/AST/node_cast.hpp>

#include <memory>
#include <iostream>
#include <exception>


namespace Veriparse {
namespace Importers {

AST::Node::Ptr YAMLImporter::import(const char *filename) const {
	YAML::Node node;

	try {
		node = YAML::LoadFile(filename);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::import(std::istream &iss) const {
	YAML::Node node;

	try {
		node = YAML::Load(iss);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::import(const std::string &str) const {
	YAML::Node node;

	try {
		node = YAML::Load(str);
	}
	catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return convert(node);
}

AST::Node::Ptr YAMLImporter::convert(const YAML::Node node) const {
	if (node.IsMap()) {
		{%- for ntype in nodes_dict.keys() %}
		if (node["{{ ntype }}"]) {
			return convert_{{ ntype.lower() }}(node["{{ ntype }}"]);
		}
		{%- endfor %}
	}

	return AST::Node::Ptr(nullptr);
}

AST::Node::Ptr YAMLImporter::convert_node(const YAML::Node node) const {
	AST::Node::Ptr result(new AST::Node);
	return result;
}

{% for ntype, ndict in nodes_dict.items() %}
{%- set inherited_flat = flatten_inherited(ndict['inherited']) %}
{%- set children_dict_full = merge_dict(ndict['children'], inherited_flat['children']) -%}
{%- set properties_dict_full = merge_dict(ndict['properties'], inherited_flat['properties']) -%}
{%- set properties_user_enum_full = merge_dict(ndict['properties_user_enum'], inherited_flat['properties_user_enum']) %}

AST::Node::Ptr YAMLImporter::convert_{{ ntype.lower() }}(const YAML::Node node) const {
	AST::{{ get_type(ntype) }} result;
	if (node.IsMap()) {
		if (node["filename"]) {
			if (node["filename"].IsScalar()) {
				if(!result) result = std::make_shared<AST::{{ ntype }}>();
				result->set_filename(node["filename"].as<std::string>());
			}
		}
		if (node["line"]) {
			if (node["line"].IsScalar()) {
				if(!result) result = std::make_shared<AST::{{ ntype }}>();
				result->set_line(node["line"].as<int>());
			}
		}
		{%- for pname, ptype in properties_dict_full.items() %}
		// Manage property {{ pname }}
		if (node["{{ pname }}"]) {
			if (node["{{ pname }}"].IsScalar()) {
				{% set user_enum_dict = properties_user_enum_full%}
				if(!result) result = std::make_shared<AST::{{ ntype }}>();
				result->set_{{ pname }}(node["{{ pname }}"].as<
					{%- if ptype in ndict['properties_user_enum'] -%}AST::{{ ntype }}::
					{%- else -%}
					{%- if ptype in inherited_flat['properties_user_enum'] -%}AST::
					{%- endif -%}
					{%- endif -%}{{ ptype }}>());
			}
		}
		{%- endfor %}

		{%- for cname, ctype in children_dict_full.items() %}
		{%- set ctype_under = get_underlying_type(ctype) %}
		{%- set ctype_list = ctype_under + "::List" %}
		{%- set ctype_ptr = get_type(ctype_under) %}

		// Manage Child {{ cname }}
		if (node["{{ cname }}"]) {
			const YAML::Node node_{{ cname }} = node["{{ cname }}"];
			{%- if is_type_list(ctype) %}
			// Fill the list of children
			AST::{{ get_type(ctype) }} {{ cname }}_list (new AST::{{ ctype_list }});
			if (node_{{ cname }}.IsSequence()) {
				// The YAML node is a sequence
				for(YAML::const_iterator it=node_{{ cname }}.begin(); it !=  node_{{ cname }}.end(); ++it) {
					AST::Node::Ptr child = convert(*it);
					if (child) {
						{%- if ctype_ptr != "Node::Ptr" %}
						AST::{{ ctype_ptr }} child_cast = AST::cast_to<AST::{{ ctype_under }}>(child);
						{{ cname }}_list->push_back(child_cast);
						{%- else %}
						{{ cname }}_list->push_back(child);
						{%- endif %}
					}
				}
			}
			else {
				AST::Node::Ptr child = convert(node_{{ cname }});
				if (child) {
					{%- if ctype_ptr != "Node::Ptr" %}
					AST::{{ ctype_ptr }} child_cast = AST::cast_to<AST::{{ ctype_under }}>(child);
					{{ cname }}_list->push_back(child_cast);
					{%- else %}
					{{ cname }}_list->push_back(child);
					{%- endif %}
				}
			}
			// Set the list
			if(!result) result = std::make_shared<AST::{{ ntype }}>();
			result->set_{{ cname }}({{ cname }}_list);
			{%- else %}
			// Set the child
			AST::Node::Ptr child = convert(node_{{ cname }});
			if (child) {
				{%- if ctype_ptr != "Node::Ptr" %}
				AST::{{ ctype_ptr }} child_cast = AST::cast_to<AST::{{ ctype_under }}>(child);
				if(!result) result = std::make_shared<AST::{{ ntype }}>();
				result->set_{{ cname }}(child_cast);
				{%- else %}
				if(!result) result = std::make_shared<AST::{{ ntype }}>();
				result->set_{{ cname }}(child);
				{%- endif %}
			}
			{%- endif %}
		}
		{%- endfor %}
	}

	// Return the result
	return AST::cast_to<AST::{{ ntype }}>(result);
}
{% endfor %}

}
}
