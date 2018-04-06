#ifndef VERIPARSE_PASSES_ANALYSIS_MODULE
#define VERIPARSE_PASSES_ANALYSIS_MODULE

#include <vector>
#include <map>
#include <string>

#include <veriparse/AST/node_cast.hpp>
#include <veriparse/AST/nodes.hpp>

#include <veriparse/passes/analysis/search.hpp>


namespace Veriparse {
	namespace Passes {
		namespace Analysis {

			class Module: public StandardSearch {
			public:
				Module() = delete;

				/**
				 * Return all module nodes
				 */
				static AST::Module::ListPtr get_module_nodes(AST::Node::Ptr node);

				/**
				 * Return all module names
				 */
				static std::vector<std::string> get_module_names(AST::Node::Ptr node);

				/**
				 * Return all parameters nodes.
				 */
				static AST::Parameter::ListPtr get_parameter_nodes(AST::Node::Ptr node);

				/**
				 * Return all parameters names.
				 */
				static std::vector<std::string> get_parameter_names(AST::Node::Ptr node);

				/**
				 * Return all localparams nodes.
				 */
				static AST::Localparam::ListPtr get_localparam_nodes(AST::Node::Ptr node);

				/**
				 * Return all localparams names.
				 */
				static std::vector<std::string> get_localparam_names(AST::Node::Ptr node);

				/**
				 * Return all functions nodes.
				 */
				static AST::Function::ListPtr get_function_nodes(AST::Node::Ptr node);

				/**
				 * Return all functions names.
				 */
				static std::vector<std::string> get_function_names(AST::Node::Ptr node);

				/**
				 * Return all tasks nodes.
				 */
				static AST::Task::ListPtr get_task_nodes(AST::Node::Ptr node);

				/**
				 * Return all tasks names.
				 */
				static std::vector<std::string> get_task_names(AST::Node::Ptr node);

				/**
				 * Return all task call nodes.
				 */
				static AST::TaskCall::ListPtr get_taskcall_nodes(AST::Node::Ptr node);

				/**
				 * Return all task call names.
				 */
				static std::vector<std::string> get_taskcall_names(AST::Node::Ptr node);

				/**
				 * Return all system call nodes.
				 */
				static AST::SystemCall::ListPtr get_systemcall_nodes(AST::Node::Ptr node);

				/**
				 * Return all system call names.
				 */
				static std::vector<std::string> get_systemcall_names(AST::Node::Ptr node);

				/**
				 * Return all function call nodes.
				 */
				static AST::FunctionCall::ListPtr get_functioncall_nodes(AST::Node::Ptr node);

				/**
				 * Return all function call names.
				 */
				static std::vector<std::string> get_functioncall_names(AST::Node::Ptr node);

				/**
				 * Return all lvalues nodes.
				 */
				static AST::Lvalue::ListPtr get_lvalue_nodes(AST::Node::Ptr node);

				/**
				 * Return all lvalues identifier nodes.
				 */
				static AST::Identifier::ListPtr get_lvalue_identifier_nodes(AST::Node::Ptr node);

				/**
				 * Return all lvalues identifier names.
				 */
				static std::vector<std::string> get_lvalue_identifier_names(AST::Node::Ptr node);

				/**
				 * Return all rvalues nodes.
				 */
				static AST::Rvalue::ListPtr get_rvalue_nodes(AST::Node::Ptr node);

				/**
				 * Return all rvalues identifier nodes.
				 */
				static AST::Identifier::ListPtr get_rvalue_identifier_nodes(AST::Node::Ptr node);

				/**
				 * Return all rvalues identifier names.
				 */
				static std::vector<std::string> get_rvalue_identifier_names(AST::Node::Ptr node);

				/**
				 * Return all input, output and inout port nodes.
				 */
				static AST::IODir::ListPtr get_iodir_nodes(AST::Node::Ptr node);

				/**
				 * Return all input, output and inout port names.
				 */
				static std::vector<std::string> get_iodir_names(AST::Node::Ptr node);

				/**
				 * Return all output nodes.
				 */
				static AST::Output::ListPtr get_output_nodes(AST::Node::Ptr node);

				/**
				 * Return all output names.
				 */
				static std::vector<std::string> get_output_names(AST::Node::Ptr node);

				/**
				 * Return all inout nodes.
				 */
				static AST::Inout::ListPtr get_inout_nodes(AST::Node::Ptr node);

				/**
				 * Return all inout names.
				 */
				static std::vector<std::string> get_inout_names(AST::Node::Ptr node);

				/**
				 * Return all input nodes.
				 */
				static AST::Input::ListPtr get_input_nodes(AST::Node::Ptr node);

				/**
				 * Return all input names
				 */
				static std::vector<std::string> get_input_names(AST::Node::Ptr node);

				/**
				 * Return all variables (excluding genvar) nodes.
				 */
				static AST::Variable::ListPtr get_variable_nodes(AST::Node::Ptr node);

				/**
				 * Return all variables (excluding genvar) names.
				 */
				static std::vector<std::string> get_variable_names(AST::Node::Ptr node);

				/**
				 * Return all input, output and inout port nodes that are not related to an IOPort.
				 */
				static AST::Variable::ListPtr get_variable_nodes_within_module(AST::Node::Ptr node);

				/**
				 * Return all input, output and inout port names that are not related to an IOPort.
				 */
				static std::vector<std::string> get_variable_names_within_module(AST::Node::Ptr node);

				/**
				 * Return all instance nodes.
				 */
				static AST::Instance::ListPtr get_instance_nodes(AST::Node::Ptr node);

			};

		}
	}
}

#endif
