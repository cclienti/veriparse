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
				/**
				 * @brief Method that process the current node.
				 * @return zero on success.
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) = 0;

				/**
				 * @brief Call the process method for each child (and the
				 * current node become the parent).
				 * @return zero on success.
				 */
				virtual int recurse_in_childs(AST::Node::Ptr node)
				{
					int ret = 0;
					if(node) {
						AST::Node::ListPtr children = node->get_children();
						for (AST::Node::Ptr child: *children) {
							ret += process(child, node);
						}
					}
					return ret;
				}
			};

		}
	}
}

#endif
