#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_OBFUSCATOR
#define VERIPARSE_PASSES_TRANSFORMATIONS_OBFUSCATOR

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>
#include <veriparse/passes/transformations/ast_replace.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>

#include <string>
#include <cstddef>


namespace Veriparse {
namespace Passes {
namespace Transformations {


class ModuleObfuscator: public TransformationBase
{
public:
	ModuleObfuscator(std::size_t identifier_length=16, bool use_sha1=false):
		m_identifier_length (identifier_length),
		m_use_hash (use_sha1)
	{}

private:
	/**
	 * @return zero on success
	 */
	virtual int process(AST::Node::Ptr node, AST::Node::Ptr parent) override;

	std::string push_decl(AST::Node::Ptr decl, const std::string &decl_name,
	                      bool override_collision=false);

	int rename_locals(const AST::Node::Ptr &node);

	int rename_procs(const AST::Node::Ptr &node);

private:
	const std::size_t m_identifier_length;
	const bool m_use_hash;

	Analysis::UniqueDeclaration::IdentifierSet m_unique_id;
	ASTReplace::ReplaceMap m_replace_map;
};

}
}
}

#endif
