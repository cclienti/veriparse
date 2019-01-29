#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_AST_REPLACE
#define VERIPARSE_PASSES_TRANSFORMATIONS_AST_REPLACE

#include <veriparse/AST/nodes.hpp>
#include <map>
#include <string>


namespace Veriparse {
namespace Passes {
namespace Transformations {

struct ASTReplace {
	typedef std::map<std::string, AST::Node::Ptr> ReplaceMap;

	/**
	 * Walk in the AST and replace all matching identifier
	 * name by a clone of the given value.
	 */
	static int replace_identifier(AST::Node::Ptr node, const std::string &id_name,
	                              AST::Node::Ptr value, AST::Node::Ptr parent=nullptr);

	/**
	 * Walk in the AST and replace all matching identifier
	 * name by a clone of the given value.
	 */
	static int replace_identifier(AST::Node::ListPtr node_list,
	                              const std::string &id_name, AST::Node::Ptr value);

	/**
	 * Walk in the AST and replace all matching identifier
	 * names (stored in the map as a key) by a clone of the
	 * given value (stored in the map as a value).
	 */
	static int replace_identifier(AST::Node::Ptr node, const ReplaceMap &replace_map,
	                              AST::Node::Ptr parent=nullptr);

	/**
	 * Walk in the AST and replace all matching identifier
	 * names (stored in the map as a key) by a clone of the
	 * given value (stored in the map as a value).
	 */
	static int replace_identifier(AST::Node::ListPtr node_list, const ReplaceMap &replace_map);

};

}
}
}

#endif
