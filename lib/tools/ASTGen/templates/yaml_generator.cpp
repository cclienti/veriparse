#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/importers/yaml_specializations.hpp>



namespace Veriparse {
	namespace Generators {

		YAML::Node YAMLGenerator::render_node(const AST::Node::Ptr node) const {
			YAML::Node result;
			return result;
		}
{%- for ntype, ndict in nodes_dict.items() %}
  {%  set inherited_flat = flatten_inherited(ndict['inherited']) %}
  {%- set children_dict_full = merge_dict(ndict['children'], inherited_flat['children']) -%}
  {%- set properties_dict_full = merge_dict(ndict['properties'], inherited_flat['properties']) -%}
  {%- set properties_user_enum_full = merge_dict(ndict['properties_user_enum'], inherited_flat['properties_user_enum']) %}

	 	YAML::Node YAMLGenerator::render_{{ ntype.lower() }}(const AST::{{ ntype }}::Ptr node) const {
			YAML::Node node_{{ ntype.lower() }};
			YAML::Node content;

			if (node) {
				if (node->get_node_type() != AST::NodeType::{{ ntype }}) return render(AST::cast_to<AST::Node>(node));

				content["filename"] = node->get_filename();
				content["line"] = node->get_line();
  {%- for pname, ptype in properties_dict_full.items() %}
    {%- if ptype in properties_user_enum_full %}
				switch(node->get_{{ pname }}()) {
      {%- for enum_value in properties_user_enum_full[ptype] %}
        {%- if not loop.last %}
				case Veriparse::AST::{{ ntype }}::{{ ptype }}::{{ enum_value }}:
					content["{{ pname }}"] = "{{ enum_value }}";
					break;
        {%- else %}
				default: content["{{ pname }}"] = "{{ enum_value }}";
        {%- endif %}
      {%- endfor %}
				}
    {%- else %}
				content["{{ pname }}"] = node->get_{{ pname }}();
    {%- endif %}
  {%- endfor %}

  {%- for cname, ctype in children_dict_full.items() %}
    {% if is_type_list(ctype) %}
				if (node->get_{{ cname }}()) {
					content["{{ cname }}"] = YAML::Load("[]");
					for(const AST::{{ get_type(get_underlying_type(ctype)) }} &n: *node->get_{{ cname }}()) {
						content["{{ cname }}"].push_back(render(n));
					}
				}
    {%- else %}
				content["{{ cname }}"] = render(node->get_{{ cname }}());
    {%- endif %}
  {%- endfor %}
			}

			node_{{ ntype.lower() }}["{{ ntype }}"] = content;
			return node_{{ ntype.lower() }};
		}
{% endfor %}

	}
}
