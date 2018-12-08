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

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

private:
	/**
	 * Inline localparams rvalue in all others parameters. The
	 * algorithm is in O(n^2). For each localparam, we replace
	 * the localparam rvalue in all other localparams.
	 *
	 * If there is circular dependencies, the method will
	 * fail.
	 *
	 * @return zero on success.
	 */
	int resolve_localparamlist();

	/**
	 * Remove the localparam using its name.
	 * @return zero on success.
	 */
	int remove_localparam(const AST::Node::Ptr &node, const std::string &name, const AST::Node::Ptr &parent);

private:
	AST::Localparam::ListPtr m_localparamlist;

};

}
}
}

#endif
