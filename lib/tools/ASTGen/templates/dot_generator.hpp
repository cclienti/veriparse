#ifndef VERIPARSE_GENERATORS_DOT_GENERATOR_HPP
#define VERIPARSE_GENERATORS_DOT_GENERATOR_HPP

#include <veriparse/generators/generator_base.hpp>


namespace Veriparse {
	namespace Generators {

		class DotGenerator : public GeneratorBase<std::string> {
		public:
			static void save_dot(const std::string &dot, const std::string &filename);

		private:
			virtual std::string render_node(const AST::Node::Ptr node) const;

			{%- for ntype in nodes_dict.keys() %}
			virtual std::string render_{{ ntype.lower() }}(const AST::{{ ntype }}::Ptr node) const;
			{%- endfor %}
		};

	}
}


#endif
