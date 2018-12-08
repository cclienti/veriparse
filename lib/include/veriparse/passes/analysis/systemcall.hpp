#ifndef VERIPARSE_PASSES_ANALYSIS_SYSTEM_CALL
#define VERIPARSE_PASSES_ANALYSIS_SYSTEM_CALL

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

class SystemCall: public StandardSearch {
public:
	SystemCall() = delete;

	/**
	 * Return identifier nodes from a system call
	 */
	static AST::Identifier::ListPtr get_argument_identifier_nodes(AST::SystemCall::Ptr systemcall);

	/**
	 * Return identifier names from a system call
	 */
	static std::vector<std::string> get_argument_identifier_names(AST::SystemCall::Ptr systemcall);
};

}
}
}


#endif
