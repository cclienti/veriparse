#include <veriparse/passes/transformations/branch_selection.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/annotate_declaration.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse
{
	namespace Passes
	{
		namespace Transformations
		{

			int BranchSelection::process(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				int ret = 0;

				if (node) {
				}

				return ret;
			}

		}
	}
}
