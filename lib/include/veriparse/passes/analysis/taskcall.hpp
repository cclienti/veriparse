#ifndef VERIPARSE_PASSES_ANALYSIS_TASK_CALL
#define VERIPARSE_PASSES_ANALYSIS_TASK_CALL

#include <vector>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			class TaskCall: public StandardSearch {
			public:
				TaskCall() = delete;

				/**
				 * Return identifier nodes from a task call
				 */
				static AST::Identifier::ListPtr get_argument_identifier_nodes(AST::TaskCall::Ptr taskcall);

				/**
				 * Return identifier names from a task call
				 */
				static std::vector<std::string> get_argument_identifier_names(AST::TaskCall::Ptr taskcall);

			};

		}
	}
}


#endif
