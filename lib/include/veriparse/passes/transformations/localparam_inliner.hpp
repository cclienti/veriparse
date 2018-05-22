#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_LOCALPARAM_INLINER
#define VERIPARSE_PASSES_TRANSFORMATIONS_LOCALPARAM_INLINER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class LocalparamInliner: public TransformationBase {
			public:
				LocalparamInliner();

			protected:
				/**
				 * @return zero on success
				 */
				virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:
				/**
				 * Remove the localparam using its name.
				 * @return zero on success.
				 */
				int remove_localparam(AST::Node::Ptr node, std::string name, AST::Node::Ptr parent=nullptr);

			private:
				AST::Localparam::ListPtr m_localparamlist;

			};

		}
	}
}

#endif
