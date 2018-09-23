#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{


int UniqueDeclaration::analyze(const AST::Node::Ptr &node, IdentifierSet &id_set)
{
	return 0;
}

bool UniqueDeclaration::identifier_declaration_exists(const std::string identifier, const IdentifierSet &id_set)
{
	return true;
}

std::string UniqueDeclaration::get_unique_identifier(const std::string identifier_basename, IdentifierSet &id_set)
{
	return "";
}


}
}
}
