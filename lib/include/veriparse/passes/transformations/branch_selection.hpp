#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION
#define VERIPARSE_PASSES_TRANSFORMATIONS_BRANCH_SELECTION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/expression_evaluation.hpp>
#include <string>
#include <memory>
#include <vector>
#include <utility>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class BranchSelection: public TransformationBase
			{
			protected:
				/**
				 * @return zero on success.
				 */
				int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:
			};

		}
	}
}

#endif
