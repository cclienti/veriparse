#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE
#define VERIPARSE_PASSES_TRANSFORMATIONS_TRANSFORMATION_BASE

#include <veriparse/AST/nodes.hpp>
#include <string>

namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class TransformationBase {
			public:
				/**
				 * @return zero on success
				 */
				virtual int run(AST::Node::Ptr node) {
					return process(node, nullptr);
				}

			protected:
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) = 0;

			};

		}
	}
}

#endif
