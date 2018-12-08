#ifndef VERIPARSE_PASSES_ANALYSIS_IDENTIFIER
#define VERIPARSE_PASSES_ANALYSIS_IDENTIFIER

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

class Identifier: public StandardSearch {
public:
	Identifier() = delete;

	/**
	 * Return identifier nodes from an identifier
	 */
	static AST::Identifier::ListPtr get_identifier_names(AST::Identifier::Ptr identifier);

	/**
	 * Return identifier nodes from an identifier
	 */
	static AST::Identifier::ListPtr get_identifier_names(AST::Identifier::Ptr identifier);


};

}
}
}

#endif
