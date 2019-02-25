#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_ANNOTATE_SCOPE
#define VERIPARSE_PASSES_TRANSFORMATIONS_ANNOTATE_SCOPE

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

#include <regex>
#include <string>
#include <map>


namespace Veriparse {
namespace Passes {
namespace Transformations {

/**
 * This pass provides an annotation scheme of scope within a
 * module. It can ignore scopes related to an
 * Input/Output/Inout.
 */
class AnnotateScope: public TransformationBase {
	typedef std::map<std::string, std::string> ReplaceDict;

public:
	/**
	 * Default constructor. The default search pattern is
	 * "^.*$" and the default replace pattern is "$&".
	 */
	AnnotateScope();

	/**
	 * Constructor with the regex search (c++11 format) and
	 * replace pattern.
	 */
	AnnotateScope(const std::string &search, const std::string &replace);

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
	int get_scope_names(const AST::Node::Ptr node, ReplaceDict &replace_dict);

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
