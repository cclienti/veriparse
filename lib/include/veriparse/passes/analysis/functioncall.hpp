#ifndef VERIPARSE_PASSES_ANALYSIS_FUNCTION_CALL
#define VERIPARSE_PASSES_ANALYSIS_FUNCTION_CALL

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

class FunctionCall: public StandardSearch {
public:
	FunctionCall() = delete;

	/**
	 * Return identifier nodes from a function call
	 */
	static AST::Identifier::ListPtr get_argument_identifier_nodes(AST::FunctionCall::Ptr functioncall);

	/**
	 * Return identifier names from a function call
	 */
	static std::vector<std::string> get_argument_identifier_names(AST::FunctionCall::Ptr functioncall);
};

}
}
}

#endif
