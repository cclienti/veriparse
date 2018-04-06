#ifndef VERIPARSE_GENERATORS_GENERATOR_BASE
#define VERIPARSE_GENERATORS_GENERATOR_BASE

#include <veriparse/AST/nodes.hpp>

namespace Veriparse {
	namespace Generators {

		template <typename T>
		class GeneratorBase {
		public:

			/**
			 * Constructor, can use tabs or spaces to indent.
			 */
			GeneratorBase(bool use_tabs=false, unsigned int indent_length=2) {
				if (use_tabs) {
					m_indent_str.assign(1, '\t');
				}
				else {
					m_indent_str.assign(indent_length, ' ');
				}
			}

			virtual ~GeneratorBase(void) {}

			/**
			 * Return the indent string built using constructor
			 * parameters.
			 */
			const std::string &get_indent_str(void) const {
				return m_indent_str;
			}

			/**
			 * Main rendering method. When the node type is from the base
			 * class, it dispatches the work to the right protected
			 * rendering method depending on the internal node type.
			 */
			T render(const AST::Node::Ptr node) const {
				if (node) {
					switch(node->get_node_type()) {
					{%- for ntype in node_type_list %}
					case AST::NodeType::{{ ntype }}:
					return render_{{ ntype.lower() }}(AST::cast_to<AST::{{ ntype }}>(node));
					{% endfor %}
					default: return render_node(node);
					}
				}
				else {
					return T();
				}
			}
			{% for ntype in node_type_list %}
			/**
			 * Main rendering method for the {{ ntype }} node, we
			 * dispatch directly to the right rendering method.
			 *
			 * @see GeneratorBase::render_{{ ntype.lower() }}(const AST::{{ get_type(ntype) }} &node)
			 */
			T render(const AST::{{ get_type(ntype) }} node) const {
				return render_{{ ntype.lower() }}(node);
			}
			{% endfor %}

		protected:
			/**
			 * Render the Node base class. This method must be overloaded
			 * in a derived class to your needs.
			 */
			virtual T render_node(const AST::Node::Ptr node) const {
				return T();
			}
			{% for ntype in node_type_list %}
			/**
			 * Render the {{ ntype }} node. This method must be
			 * overloaded in a derived class to your needs.
			 */
			virtual T render_{{ ntype.lower() }}(const AST::{{ get_type(ntype) }} node) const {
				return T();
			}
			{% endfor %}


		private:
			std::string m_indent_str;

		};

	}
}

#endif
