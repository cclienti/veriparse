#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION
#define VERIPARSE_PASSES_TRANSFORMATIONS_DEADCODE_ELIMINATION

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <set>
#include <string>


namespace Veriparse {
	namespace Passes {
		namespace Transformations {

			class DeadcodeElimination: public TransformationBase
			{
			public:
				typedef std::set<std::string> DSet;

			protected:
				int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

			private:
				DSet remove_deadcode_step(AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_deadstmt(const DeadcodeElimination::DSet &deadset,
				                    DeadcodeElimination::DSet &removedset,
				                    AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_deaddecl(const DeadcodeElimination::DSet &removedset,
				                    AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_emptyblock(AST::Node::Ptr node, AST::Node::Ptr parent);

				int remove_emptystmt(AST::Node::Ptr node, AST::Node::Ptr parent);

				template<typename T>
				std::set<T> vec2set(const std::vector<T> &v)	{return std::set<T>(v.cbegin(), v.cend());}
			};

		}
	}
}

#endif
