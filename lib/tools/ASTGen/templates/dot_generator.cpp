#include <veriparse/generators/dot_generator.hpp>
#include <sstream>
#include <fstream>
#include <cstdint>


namespace Veriparse {
	namespace Generators {

		void DotGenerator::save_dot(const std::string &dot, const std::string &filename) {
			std::ofstream fout(filename);
			fout << "digraph G {\n"
			     << "\trankdir=LR;\n"
			     << "\tnode [shape=plaintext];\n"
			     << dot << "}" << std::endl;
		}


		std::string DotGenerator::render_node(const AST::Node::Ptr node) const {
			std::stringstream ss;

			if (node) {
				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());
				ss << "\tn" << nodeID << " [label=\"{<p0>Node}"
				   << " | {line: " << node->get_line() << "}" << "\"];" << std::endl;
			}

			return ss.str();
		}
		{% for ntype, ndict in nodes_dict.items() %}
		{%  set inherited_flat = flatten_inherited(ndict['inherited']) %}
		{%- set children_dict_full = merge_dict(ndict['children'], inherited_flat['children']) -%}
		{%- set properties_dict_full = merge_dict(ndict['properties'], inherited_flat['properties']) -%}
		{%- set properties_user_enum_full = merge_dict(ndict['properties_user_enum'], inherited_flat['properties_user_enum']) %}

		std::string DotGenerator::render_{{ ntype.lower() }}(const AST::{{ ntype }}::Ptr node) const {
			std::stringstream ss;

			if (node) {
				if (node->get_node_type() != AST::NodeType::{{ ntype }}) return render(AST::cast_to<AST::Node>(node));

				uint64_t nodeID = reinterpret_cast<uint64_t>(node.get());

				ss << "\tn" << nodeID
				   << " [label=< <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"4\">\n"
				   << "\t\t<TR><TD PORT=\"p0\" BGCOLOR=\"gray10\">"
				   << "<FONT COLOR=\"white\">{{ ntype }}</FONT></TD></TR>\n"
				   {%- if ntype.lower() == "source" %}
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">file: " << node->get_filename() << "</TD></TR>\n"
				   {%- endif %}
				   << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">line: " << node->get_line() << "</TD></TR>\n";

				{%- for pname, ptype in properties_dict_full.items() %}
				  {%- if ptype in properties_user_enum_full %}
				switch(node->get_{{ pname }}()) {
					{%- for enum_value in properties_user_enum_full[ptype] %}
						{%- if not loop.last %}
				case Veriparse::AST::{{ ntype }}::{{ ptype }}::{{ enum_value }}:
					ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">{{ pname }}: {{ enum_value }}</TD></TR>\n";
					break;
						{%- else %}
				default: ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">{{ pname }}: {{ enum_value }}</TD></TR>\n";
						{%- endif %}
					{%- endfor %}
				}
				  {%- else %}
				  ss << "\t\t<TR><TD BGCOLOR=\"cornsilk2\">{{ pname }}: "
				     << node->get_{{ pname }}() << "</TD></TR>\n";
				  {%- endif %}
				{%- endfor %}


				{%- for cname, ctype in children_dict_full.items() %}
				ss << "\t\t<TR><TD PORT=\"p{{ loop.index }}\" BGCOLOR=\"darkslategray\">"
				   << "<FONT COLOR=\"wheat\">{{ cname }}</FONT></TD></TR>\n";
				{%- endfor %}

				ss << "\t\t</TABLE>>];" << std::endl;

				{%- for cname, ctype in children_dict_full.items() %}
				{%- if is_type_list(ctype) %}
				if (node->get_{{ cname }}()) {
					for(const AST::{{ get_type(get_underlying_type(ctype)) }} &n: *node->get_{{ cname }}()) {
						if (n) {
							ss << render(n);
						}
					}
				}
				{%- else %}
				if (node->get_{{ cname }}().get()) {
					ss << render(node->get_{{ cname }}());
				}
				{%- endif %}
				{%- endfor %}

				{%- for cname, ctype in children_dict_full.items() %}
				{%- if loop.first %}
				uint64_t childID;
				{%- endif %}
				{%- if is_type_list(ctype) %}
				if (node->get_{{ cname }}()) {
					int i=0;
					for(const AST::{{ get_type(get_underlying_type(ctype)) }} &n: *node->get_{{ cname }}()) {
						childID = reinterpret_cast<uint64_t>(n.get());
						if (childID) {
							ss << "\tn" << nodeID << ":p{{ loop.index }} -> n" << childID << " [label=\"i=" << i++ <<"\"];" << std::endl;
						}
					}
				}
				{%- else %}
				childID = reinterpret_cast<uint64_t>(node->get_{{ cname }}().get());
				if (childID) {
					ss << "\tn" << nodeID << ":p{{ loop.index }} -> n" << childID << ";" << std::endl;
				}
				{%- endif %}
				{%- endfor %}
			}

			return ss.str();
		}
		{% endfor %}

	}
}
