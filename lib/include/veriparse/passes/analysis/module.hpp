#ifndef VERIPARSE_PASSES_ANALYSIS_MODULE
#define VERIPARSE_PASSES_ANALYSIS_MODULE

#include "veriparse/AST/function.hpp"
#include <vector>
#include <map>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
namespace Passes {
namespace Analysis {

class Module: public StandardSearch
{
public:
	Module() = delete;

	using ModulesMap = std::map<std::string, AST::Module::Ptr>;
	using FunctionMap = std::map<std::string, AST::Function::Ptr>;

	/**
	 * @brief Fill the map with all modules found in the
	 * node_list.
	 *
	 * The key will be the module name string and the value
	 * the module AST.
	 */
	static int get_module_dictionary(const AST::Node::ListPtr &node_list, ModulesMap &modules_map);

	/**
	 * @brief Fill the map with all modules found in the
	 * node.
	 *
	 * The key will be the module name string and the value
	 * the module AST.
	 */
	static int get_module_dictionary(const AST::Node::Ptr &node, ModulesMap &modules_map);

	/**
	 * @brief Return all module nodes
	 */
	static AST::Module::ListPtr get_module_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all module names
	 */
	static std::vector<std::string> get_module_names(AST::Node::Ptr node);

	/**
	 * @brief Return all parameters nodes.
	 */
	static AST::Parameter::ListPtr get_parameter_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all parameters names.
	 */
	static std::vector<std::string> get_parameter_names(AST::Node::Ptr node);

	/**
	 * @brief Return all parameters nodes.
	 *
	 * It returns the port nodes in the module port declaration. It
	 * takes into account the correct order of declaration. It does not
	 * take into account the IODir nodes declared in the module
	 * statements.
	 */
	static AST::Node::ListPtr get_port_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all parameters names.
	 *
	 * It retrieves the port names in the module port declaration. It
	 * takes into account the correct order of declaration. It does not
	 * take into account the IODir names declared in the module
	 * statements.
	 */
	static std::vector<std::string> get_port_names(AST::Node::Ptr node);

	/**
	 * @brief Return all localparams nodes.
	 */
	static AST::Localparam::ListPtr get_localparam_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all localparams names.
	 */
	static std::vector<std::string> get_localparam_names(AST::Node::Ptr node);

	/**
	 * @brief Fill the map with all function definitions found in the
	 * nodes.
	 *
	 * The key will be the function name string and the value
	 * the function definition AST.
	 */
	static int get_function_dictionary(const AST::Node::ListPtr &node_list, FunctionMap &function_map);

	/**
	 * @brief Fill the map with all function definitions found in the
	 * node.
	 *
	 * The key will be the function name string and the value
	 * the function definition AST.
	 */
	static int get_function_dictionary(const AST::Node::Ptr &node, FunctionMap &function_map);

	/**
	 * @brief Return all functions nodes.
	 */
	static AST::Function::ListPtr get_function_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all functions names.
	 */
	static std::vector<std::string> get_function_names(AST::Node::Ptr node);

	/**
	 * @brief Return all tasks nodes.
	 */
	static AST::Task::ListPtr get_task_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all tasks names.
	 */
	static std::vector<std::string> get_task_names(AST::Node::Ptr node);

	/**
	 * @brief Return all task call nodes.
	 */
	static AST::TaskCall::ListPtr get_taskcall_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all task call names.
	 */
	static std::vector<std::string> get_taskcall_names(AST::Node::Ptr node);

	/**
	 * @brief Return all system call nodes.
	 */
	static AST::SystemCall::ListPtr get_systemcall_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all system call names.
	 */
	static std::vector<std::string> get_systemcall_names(AST::Node::Ptr node);

	/**
	 * @brief Return all function call nodes.
	 */
	static AST::FunctionCall::ListPtr get_functioncall_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all function call names.
	 */
	static std::vector<std::string> get_functioncall_names(AST::Node::Ptr node);

	/**
	 * @brief Return all lvalues nodes.
	 */
	static AST::Lvalue::ListPtr get_lvalue_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all rvalues nodes.
	 */
	static AST::Rvalue::ListPtr get_rvalue_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all rvalues identifier nodes.
	 */
	static AST::Identifier::ListPtr get_rvalue_identifier_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all rvalues identifier names.
	 */
	static std::vector<std::string> get_rvalue_identifier_names(AST::Node::Ptr node);

	/**
	 * @brief Return all input, output and inout port nodes.
	 */
	static AST::IODir::ListPtr get_iodir_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all input, output and inout port names.
	 */
	static std::vector<std::string> get_iodir_names(AST::Node::Ptr node);

	/**
	 * @brief Return all output nodes.
	 */
	static AST::Output::ListPtr get_output_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all output names.
	 */
	static std::vector<std::string> get_output_names(AST::Node::Ptr node);

	/**
	 * @brief Return all inout nodes.
	 */
	static AST::Inout::ListPtr get_inout_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all inout names.
	 */
	static std::vector<std::string> get_inout_names(AST::Node::Ptr node);

	/**
	 * @brief Return all input nodes.
	 */
	static AST::Input::ListPtr get_input_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all input names
	 */
	static std::vector<std::string> get_input_names(AST::Node::Ptr node);

	/**
	 * @brief Return all variables (excluding genvar) nodes.
	 */
	static AST::Variable::ListPtr get_variable_nodes(AST::Node::Ptr node);

	/**
	 * @brief Return all variables (excluding genvar) names.
	 */
	static std::vector<std::string> get_variable_names(AST::Node::Ptr node);

	/**
	 * @brief Return all variable nodes that are not related to an IOPort.
	 */
	static AST::Variable::ListPtr get_variable_nodes_within_module(AST::Node::Ptr node);

	/**
	 * @brief Return variable names that are not related to an IOPort.
	 */
	static std::vector<std::string> get_variable_names_within_module(AST::Node::Ptr node);

	/**
	 * @brief Return all instance nodes.
	 */
	static AST::Instance::ListPtr get_instance_nodes(AST::Node::Ptr node);

};

}
}
}

#endif
