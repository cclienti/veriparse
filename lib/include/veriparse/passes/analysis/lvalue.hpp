#ifndef VERIPARSE_PASSES_ANALYSIS_LVALUE
#define VERIPARSE_PASSES_ANALYSIS_LVALUE

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

class Lvalue: public StandardSearch {
public:
	Lvalue() = delete;

	/**
	 * Return all identifier nodes from an lvalue node. This include
	 * lvalues and rvalues in the left part of the statement.
	 *
	 * @sa get_identifier_names
	 * @sa get_rvalue_nodes
	 */
	static AST::Identifier::ListPtr get_identifier_nodes(AST::Lvalue::Ptr lvalue);

	/**
	 * Return all identifier names from an lvalue. This include
	 * lvalue and rvalue names in the left part of the statement.
	 *
	 * @sa get_identifier_nodes
	 * @sa get_rvalue_names
	 */
	static std::vector<std::string> get_identifier_names(AST::Lvalue::Ptr lvalue);

	/**
	 * Return lvalue identifier nodes.
	 *
	 * @sa get_lvalue_names
	 */
	static AST::Identifier::ListPtr get_lvalue_nodes(AST::Lvalue::Ptr lvalue);

	/**
	 * Return lvalue identifier names.
	 *
	 * @sa get_lvalue_nodes
	 */
	static std::vector<std::string> get_lvalue_names(AST::Lvalue::Ptr lvalue);

	/**
	 * Return rvalue identifer nodes from an lvalue node.
	 *
	 * Considering the following statement:
	 *
	 * @code
	 *   p[i] = x;
	 * @endcode
	 *
	 * The "i" variable in the lvalue "p[i]" is considred as
	 * a rvalue.
	 *
	 * @sa get_rvalue_names
	 */
	static AST::Identifier::ListPtr get_rvalue_nodes(AST::Lvalue::Ptr lvalue);

	/**
	 * Return rvalue identifier names from an lvalue node.
	 *
	 * @sa get_rvalue_nodes
	 */
	static std::vector<std::string> get_rvalue_names(AST::Lvalue::Ptr lvalue);

private:
	static void get_indirect_node_list(AST::Node::Ptr node, AST::Identifier::ListPtr list,
	                                   std::size_t occur_depth=0);
};

}
}
}

#endif
