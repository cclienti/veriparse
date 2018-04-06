#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_ANOTATE_DECLARATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_ANOTATE_DECLARATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <regex>
#include <string>
#include <set>
#include <map>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			/**
			 * This pass provides an annotation scheme of declaration
			 * with a module. It will ignore declarations related to an
			 * Input/Output/Inout.
			 */
			class AnotateDeclaration: public TransformationBase {
				typedef std::map<std::string, std::string> ReplaceDict;

			public:
				/**
				 * Default constructor. The default search pattern is
				 * "^.*$" and the default replace pattern is "$&".
				 */
				AnotateDeclaration();

				/**
				 * Constructor with the regex search (c++11 format) and
				 * replace pattern.
				 */
				AnotateDeclaration(const std::string &search, const std::string &replace);

				/**
				 * Set the regex search (c++11 format) and replace
				 * pattern.
				 */
				void set_search_replace(const std::string &search, const std::string &replace);

			protected:
				/**
				 * @return zero on success
				 */
				int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:
				/**
				 * @return zero on success
				 */
				int get_declaration_names(const AST::Node::Ptr node, ReplaceDict &replace_dict,
				                          const std::set<std::string> &excluded_names);

				/**
				 * @return zero on success
				 */
				int anotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict);

			private:
				std::regex m_search;
				std::string m_replace;
			};

		}
	}
}

#endif
