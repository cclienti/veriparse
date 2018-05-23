#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_ANNOTATE_DECLARATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_ANNOTATE_DECLARATION

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
			class AnnotateDeclaration: public TransformationBase {
				typedef std::map<std::string, std::string> ReplaceDict;

			public:
				/**
				 * Default constructor. The default search pattern is
				 * "^.*$" and the default replace pattern is "$&".
				 */
				AnnotateDeclaration();

				/**
				 * Constructor with the regex search (c++11 format) and
				 * replace pattern.
				 */
				AnnotateDeclaration(const std::string &search, const std::string &replace);

				/**
				 * Set the regex search (c++11 format) and replace
				 * pattern.
				 */
				void set_search_replace(const std::string &search, const std::string &replace);

			private:
				/**
				 * @return zero on success
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

				/**
				 * @return zero on success
				 */
				int get_declaration_names(const AST::Node::Ptr node, ReplaceDict &replace_dict,
				                          const std::set<std::string> &excluded_names);

				/**
				 * @return zero on success
				 */
				int annotate_names(AST::Node::Ptr node, ReplaceDict &replace_dict);

			private:
				std::regex m_search;
				std::string m_replace;
			};

		}
	}
}

#endif
