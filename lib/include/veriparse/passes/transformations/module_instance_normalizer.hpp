#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class ModuleInstanceNormalizer: public TransformationBase {
				/**
				 * @return zero on success
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			};

		}
	}
}

#endif
