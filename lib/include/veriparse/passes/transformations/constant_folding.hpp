#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_CONSTANT_FOLDING
#define VERIPARSE_PASSES_TRANSFORMATIONS_CONSTANT_FOLDING

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class ConstantFolding: public TransformationBase {
			protected:
				/**
				 * @return zero on success
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			};

		}
	}
}

#endif
