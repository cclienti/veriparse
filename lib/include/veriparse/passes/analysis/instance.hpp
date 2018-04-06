#ifndef VERIPARSE_PASSES_ANALYSIS_INSTANCE
#define VERIPARSE_PASSES_ANALYSIS_INSTANCE

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			class Instance: public StandardSearch {
			public:
				Instance() = delete;

				/**
				 * Return identifier nodes from an instance
				 */
				static AST::Identifier::ListPtr get_argument_identifier_nodes(AST::Instance::Ptr instance);

				/**
				 * Return identifier names from an instance
				 */
				static std::vector<std::string> get_argument_identifier_names(AST::Instance::Ptr instance);

			};

		}
	}
}

#endif
