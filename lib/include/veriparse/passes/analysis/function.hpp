#ifndef VERIPARSE_PASSES_ANALYSIS_FUNCTION
#define VERIPARSE_PASSES_ANALYSIS_FUNCTION

#include <vector>
#include <map>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

class Function: public Search<AST::NodeType::Module, AST::NodeType::Task>
{
public:
	Function() = delete;

	/**
	 * Return all input, output and inout port nodes.
	 */
	static AST::Ioport::ListPtr get_ioport_nodes(AST::Node::Ptr node);

	/**
	 * Return all input, output and inout port nodes.
	 */
	static AST::IODir::ListPtr get_iodir_nodes(AST::Node::Ptr node);

	/**
	 * Return all input, output and inout port names.
	 */
	static std::vector<std::string> get_iodir_names(AST::Node::Ptr node);

	/**
	 * Return all declared variable nodes except genvar.
	 */
	static AST::Variable::ListPtr get_variable_nodes(AST::Node::Ptr node);

	/**
	 * Return all declared variable names except genvar.
	 */
	static std::vector<std::string> get_variable_names(AST::Node::Ptr node);

};

}
}
}

#endif
