#ifndef VERIPARSE_GENERATORS_YAML_GENERATOR_HPP
#define VERIPARSE_GENERATORS_YAML_GENERATOR_HPP

#include <yaml-cpp/yaml.h>
#include <veriparse/generators/generator_base.hpp>


namespace Veriparse {
namespace Generators {

class YAMLGenerator : public GeneratorBase<YAML::Node> {
	virtual YAML::Node render_node(const AST::Node::Ptr node) const;

	{%- for ntype in nodes_dict.keys() %}
	virtual YAML::Node render_{{ ntype.lower() }}(const AST::{{ ntype }}::Ptr node) const;
	{%- endfor %}
};

}
}


#endif
