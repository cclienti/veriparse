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

				/**
				 * @brief split instance lists
				 */
				int split_lists(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

				/**
				 * @brief split instance's array
				 */
				int split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);
			};

		}
	}
}

#endif
