#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_IO_NORMALIZER
#define VERIPARSE_PASSES_TRANSFORMATIONS_MODULE_IO_NORMALIZER

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <string>


namespace Veriparse {
namespace Passes {
namespace Transformations {

class ModuleIONormalizer: public TransformationBase {
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	void remove_module_parameters(AST::Node::Ptr node, AST::Node::Ptr parent=nullptr);

	void remove_module_ioports(AST::Node::Ptr node, AST::Node::Ptr parent=nullptr);

	void remove_module_iodirs(AST::Node::Ptr node, AST::Node::Ptr parent=nullptr);

	void remove_module_variable(const std::string &variable,
	                            AST::Variable::ListPtr removed_variables,
	                            AST::Node::Ptr node, AST::Node::Ptr parent=nullptr);

	AST::Variable::Ptr create_default_net_type_variable(AST::Module::Default_nettypeEnum defnt,
	                                                    AST::Width::ListPtr widths,
	                                                    const std::string &name);
};

}
}
}

#endif
