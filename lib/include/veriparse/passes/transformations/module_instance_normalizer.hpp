#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <string>
#include <map>
#include <list>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ModuleInstanceNormalizer: public TransformationBase
{
public:
	ModuleInstanceNormalizer() = delete;

	ModuleInstanceNormalizer(const Analysis::Module::ModulesMap &modules_map);

	virtual ~ModuleInstanceNormalizer();

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @brief split instance lists
	 */
	virtual int split_lists(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief split instance's array.
	 *
	 * During this pass, pointer or partselect will be added to
	 * existing port value. To identify the pointer or partselect, the
	 * old port expression is encapsulated in a Rvalue. This will ease
	 * the replace_port_affectation sub-pass.
	 *
	 * @sa replace_port_affectation
	 */
	virtual int split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Convert all instance's port to a named ports scheme.
	 */
	virtual int set_portarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Convert all instance's parameters to a named parameters scheme.
	 */
	virtual int set_paramarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Replace concat nodes to lconcat nodes recursively.
	 */
	virtual int convert_to_lconcat(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Create identifier to replace port value.
	 *
	 * A blocking assign will be added to affect the newly created
	 * identifier with the previous port value. To identify what is
	 * really necessary to replace, the split array sub-pass adds a
	 * rvalue into the expression.
	 */
	virtual int replace_port_affectation(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

private:
	Analysis::Dimensions::DimMap m_dim_map;
	Analysis::Module::ModulesMap m_modules_map;
};

}
}
}

#endif
