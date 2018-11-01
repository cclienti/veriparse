#ifndef VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP
#define VERIPARSE_GENERATORS_YAML_SPECIALIZATION_HPP

#include <gmp.h>
#include <gmpxx.h>
#include <yaml-cpp/yaml.h>
#include <sstream>

namespace YAML {

{%- for ntype, ndict in nodes_dict.items() %}
{%- for enum_name, enum_list in ndict["properties_user_enum"].items() %}
template<>
struct convert<Veriparse::AST::{{ ntype }}::{{ enum_name }}> {
	static Node encode(const Veriparse::AST::{{ ntype }}::{{ enum_name }} &rhs) {
		switch (rhs) {
		{%- for enum_value in enum_list %}
		{%- if not loop.last %}
		case Veriparse::AST::{{ ntype }}::{{ enum_name }}::{{ enum_value }} : return Node("{{ enum_value }}");
		{%- else %}
		default: return Node("{{ enum_value }}");
		{%- endif %}
		{%- endfor %}
		}
	}

	static bool decode(const Node& node, Veriparse::AST::{{ ntype }}::{{ enum_name }} &rhs) {
		if(!node.IsScalar()) {
			return false;
		}
		{% for enum_value in enum_list %}
		if (node.as<std::string>() == "{{ enum_value }}") {
			rhs = Veriparse::AST::{{ ntype }}::{{ enum_name }}::{{ enum_value }};
			return true;
		}
		{% endfor %}
		return false;
	}
};

{%- endfor %}
{%- endfor %}

template<>
struct convert<mpz_class> {
	static Node encode(const mpz_class &rhs) {
		return Node(rhs.get_str(10));
	}

	static bool decode(const Node& node, mpz_class &rhs) {
		if(!node.IsScalar()) {
			return false;
		}

		bool result = true;

		try {
			mpz_class num(node.as<std::string>());
			rhs = num;
		}
		catch (const std::invalid_argument &arg) {
			result = false;
		}

		return result;
	}
};

}


#endif
