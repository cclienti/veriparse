#include <veriparse/passes/transformations/variable_folding.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <veriparse/logger/logger.hpp>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {


			int VariableFolding::process(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				if (node) {

				}
				return recurse_in_childs(node);
			}

			int VariableFolding::inline_blocking_assignation(AST::Node::Ptr node, AST::Node::Ptr parent)
			{
				return 0;
			}

			AST::Node::Ptr VariableFolding::analyze_rvalue(AST::Rvalue::Ptr rvalue)
			{
				return nullptr;
			}

			std::string VariableFolding::analyze_lvalue(AST::Lvalue::Ptr node)
			{
				return "";
			}


		}
	}
}
