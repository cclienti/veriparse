#ifndef VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP
#define VERIPARSE_IMPORTERS_YAML_IMPORTER_HPP


#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
	namespace Importers {

		class YAMLImporter {
		public:
			AST::Node::Ptr import(const char *filename) const;
			AST::Node::Ptr import(const std::string &str) const;
			AST::Node::Ptr import(std::istream &iss) const;

		private:
			AST::Node::Ptr convert(const YAML::Node node) const;

			AST::Node::Ptr convert_node(const YAML::Node node) const;
			{%- for ntype in nodes_dict.keys() %}
			AST::Node::Ptr convert_{{ ntype.lower() }}(const YAML::Node node) const;
			{%- endfor %}
		};

	}
}


#endif
