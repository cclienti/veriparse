#ifndef VERIPARSE_PASSES_ANALYSIS_UNIQUE_MODULE
#define VERIPARSE_PASSES_ANALYSIS_UNIQUE_MODULE

#include <vector>
#include <set>
#include <string>
#include <random>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

/**
 * @brief Analyze all declarations to be able to check unicity of an
 * identifier.
 *
 * It will collect internally all identifiers of I/O, variables,
 * instances, tasks, funtcions, ...
 *
 */
class UniqueDeclaration
{
	using RandGen = std::default_random_engine;

public:
	using IdentifierSet = std::set<std::string>;

	/**
	 * @brief Static class
	 */
	UniqueDeclaration() = delete;

	/**
	 * @brief collect in the identifier set all identifiers of I/O,
	 * variables, instances, tasks, funtcions, ...
	 *
	 * @return zero on success
	 */
	static int analyze(const AST::Node::Ptr &node, IdentifierSet &id_set);

	/**
	 * @brief Return true if the given identifier is found in the
	 * internal set.
	 */
	static bool identifier_declaration_exists(const std::string &identifier, const IdentifierSet &id_set);

	/**
	 * @brief Return a unique identifier. It can suffix the given
	 * identifier with a random number.
	 *
	 * The given identifier set will be updated with the returned
	 * identifier string.
	 */
	static std::string get_unique_identifier(const std::string &identifier_basename, IdentifierSet &id_set,
	                                         std::size_t digits=6);

	/**
	 * @brief Reset the random generator seed.
	 */
	static void seed(RandGen::result_type value);

private:
	static thread_local RandGen s_generator;

};

}
}
}

#endif
