#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>
#include <set>

namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class DeadcodeElimination: public TransformationBase
			{
				typedef std::set<std::string> DSet;

			protected:
				int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:
				std::string print_set(const DSet &dset);

				DSet remove_deadcode_step(AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_deadstmt(const DeadcodeElimination::DSet &deadset,
				                    DeadcodeElimination::DSet &removedset,
				                    AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_deaddecl(const DeadcodeElimination::DSet &removedset,
				                    AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_emptyblock(AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_emptystmt(AST::Node::Ptr node, AST::Node::Ptr parent);
			};

		}
	}
}

#endif
