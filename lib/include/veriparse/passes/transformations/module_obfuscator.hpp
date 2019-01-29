#ifndef VERIPARSE_PASSES_TRANSFORMATIONS_OBFUSCATOR
#define VERIPARSE_PASSES_TRANSFORMATIONS_OBFUSCATOR

#include <veriparse/AST/nodes.hpp>
#include <veriparse/passes/transformations/transformation_base.hpp>

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

private:
	const std::size_t m_identifier_length;
	const bool m_use_hash;
};

}
}
}

#endif
