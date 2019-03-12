#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_INSTANCE_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/dimensions.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <string>
#include <map>
#include <list>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ModuleInstanceNormalizer: public TransformationBase
{
public:
	ModuleInstanceNormalizer(const Analysis::Module::ModulesMap &modules_map);

	virtual ~ModuleInstanceNormalizer();

private:
	/**
	 * @return zero on success
	 */
	int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	/**
	 * @brief split instance lists
	 */
	int split_lists(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

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
	int split_array(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Convert all instance's port to a named ports scheme.
	 */
	int set_portarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Analysis defparam declaration.
	 *
	 * Fill m_defparam_map with gathered defparam. The map key is the
	 * rendered string that corresponds to the first scope element
	 * (front in list). The value is the defparam node itself.
	 *
	 * Only defparam with an identifier with one scope element are
	 * taken into account (ie: "defparam scope.inst = X").
	 */
	int fill_defparam_map(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Convert all instance's parameters to a named parameters scheme.
	 */
	int set_paramarg_names(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Replace concat nodes to lconcat nodes recursively.
	 */
	int convert_to_lconcat(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

	/**
	 * @brief Create identifier to replace port value.
	 *
	 * A blocking assign will be added to affect the newly created
	 * identifier with the previous port value. To identify what is
	 * really necessary to replace, the split array sub-pass adds a
	 * rvalue into the expression.
	 */
	int replace_port_affectation(const AST::Node::Ptr &node, const AST::Node::Ptr &parent);

private:
	Analysis::Dimensions::DimMap m_dim_map;
	Analysis::Module::ModulesMap m_modules_map;
	Analysis::UniqueDeclaration::IdentifierSet m_declared;
	std::map<std::string, AST::Defparam::Ptr> m_defparam_map;
};

}
}
}

#endif
