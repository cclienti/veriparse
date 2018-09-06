#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/logger/logger.hpp>
#include <algorithm>


namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

int Module::get_module_dictionary(const AST::Node::ListPtr &node_list, StrToModule &str_to_module)
{
	if (!node_list) {
		return 1;
	}

	for (const auto &node: *node_list) {
		const auto &nodes = get_module_nodes(node);
		for (const auto module: *nodes) {
			if (str_to_module.count(module->get_name()) != 0)
			{
				LOG_ERROR_N(module) << "module " << module->get_name() << " already declared";
				LOG_ERROR_N(str_to_module[module->get_name()]) << "module " << module->get_name() << " was firstly found here";
				return 1;
			}
			str_to_module.emplace(module->get_name(), module);
		}
	}

	return 0;
}


AST::Module::ListPtr Module::get_module_nodes(AST::Node::Ptr node)
{
	AST::Module::ListPtr list = std::make_shared<AST::Module::List>();
	get_node_list<AST::Module>(node, AST::NodeType::Module, list);
	return list;
}

std::vector<std::string> Module::get_module_names(AST::Node::Ptr node)
{
	AST::Module::ListPtr modules = get_module_nodes(node);
	return get_property_in_list<std::string, AST::Module>
		(modules, [](AST::Module::Ptr n){return n->get_name();});
}



AST::Parameter::ListPtr Module::get_parameter_nodes(AST::Node::Ptr node)
{
	AST::Parameter::ListPtr list = std::make_shared<AST::Parameter::List>();
	get_node_list<AST::Parameter>(node, AST::NodeType::Parameter, list);
	return list;
}

std::vector<std::string> Module::get_parameter_names(AST::Node::Ptr node)
{
	AST::Parameter::ListPtr parameters = get_parameter_nodes(node);
	return get_property_in_list<std::string, AST::Parameter>
		(parameters, [](AST::Parameter::Ptr n){return n->get_name();});
}



AST::Localparam::ListPtr Module::get_localparam_nodes(AST::Node::Ptr node)
{
	AST::Localparam::ListPtr list = std::make_shared<AST::Localparam::List>();
	get_node_list<AST::Localparam>(node, AST::NodeType::Localparam, list);
	return list;
}

std::vector<std::string> Module::get_localparam_names(AST::Node::Ptr node)
{
	AST::Localparam::ListPtr localparams = get_localparam_nodes(node);
	return get_property_in_list<std::string, AST::Localparam>
		(localparams, [](AST::Localparam::Ptr n){return n->get_name();});
}



AST::Function::ListPtr Module::get_function_nodes(AST::Node::Ptr node)
{
	AST::Function::ListPtr list = std::make_shared<AST::Function::List>();
	get_node_list<AST::Function>(node, AST::NodeType::Function, list);
	return list;
}

std::vector<std::string> Module::get_function_names(AST::Node::Ptr node)
{
	AST::Function::ListPtr functions = get_function_nodes(node);
	return get_property_in_list<std::string, AST::Function>
		(functions, [](AST::Function::Ptr n){return n->get_name();});
}



AST::Task::ListPtr Module::get_task_nodes(AST::Node::Ptr node)
{
	AST::Task::ListPtr list = std::make_shared<AST::Task::List>();
	get_node_list<AST::Task>(node, AST::NodeType::Task, list);
	return list;
}

std::vector<std::string> Module::get_task_names(AST::Node::Ptr node)
{
	AST::Task::ListPtr tasks = get_task_nodes(node);
	return get_property_in_list<std::string, AST::Task>
		(tasks, [](AST::Task::Ptr n){return n->get_name();});
}



AST::TaskCall::ListPtr Module::get_taskcall_nodes(AST::Node::Ptr node)
{
	AST::TaskCall::ListPtr list = std::make_shared<AST::TaskCall::List>();
	get_node_list<AST::TaskCall>(node, AST::NodeType::TaskCall, list);
	return list;
}

std::vector<std::string> Module::get_taskcall_names(AST::Node::Ptr node)
{
	AST::TaskCall::ListPtr taskcalls = get_taskcall_nodes(node);
	return get_property_in_list<std::string, AST::TaskCall>
		(taskcalls, [](AST::TaskCall::Ptr n){return n->get_name();});
}



AST::SystemCall::ListPtr Module::get_systemcall_nodes(AST::Node::Ptr node)
{
	AST::SystemCall::ListPtr list = std::make_shared<AST::SystemCall::List>();
	get_node_list<AST::SystemCall>(node, AST::NodeType::SystemCall, list);
	return list;
}

std::vector<std::string> Module::get_systemcall_names(AST::Node::Ptr node)
{
	AST::SystemCall::ListPtr systemcalls = get_systemcall_nodes(node);
	return get_property_in_list<std::string, AST::SystemCall>
		(systemcalls, [](AST::SystemCall::Ptr n){return n->get_syscall();});
}



AST::FunctionCall::ListPtr Module::get_functioncall_nodes(AST::Node::Ptr node)
{
	AST::FunctionCall::ListPtr list = std::make_shared<AST::FunctionCall::List>();
	get_node_list<AST::FunctionCall>(node, AST::NodeType::FunctionCall, list);
	return list;
}

std::vector<std::string> Module::get_functioncall_names(AST::Node::Ptr node)
{
	AST::FunctionCall::ListPtr functioncalls = get_functioncall_nodes(node);
	return get_property_in_list<std::string, AST::FunctionCall>
		(functioncalls, [](AST::FunctionCall::Ptr n){return n->get_name();});
}



AST::Lvalue::ListPtr Module::get_lvalue_nodes(AST::Node::Ptr node)
{
	AST::Lvalue::ListPtr list = std::make_shared<AST::Lvalue::List>();
	get_node_list<AST::Lvalue>(node, AST::NodeType::Lvalue, list);
	return list;
}



AST::Rvalue::ListPtr Module::get_rvalue_nodes(AST::Node::Ptr node)
{
	AST::Rvalue::ListPtr list = std::make_shared<AST::Rvalue::List>();
	get_node_list<AST::Rvalue>(node, AST::NodeType::Rvalue, list);
	return list;
}

AST::Identifier::ListPtr Module::get_rvalue_identifier_nodes(AST::Node::Ptr node)
{
	AST::Rvalue::ListPtr rvalues = Module::get_rvalue_nodes(node);
	AST::Identifier::ListPtr list = std::make_shared<AST::Identifier::List>();
	for(AST::Rvalue::Ptr rvalue: *rvalues) {
		get_node_list<AST::Identifier>(rvalue, AST::NodeType::Identifier, list);
	}
	return list;
}

std::vector<std::string> Module::get_rvalue_identifier_names(AST::Node::Ptr node)
{
	AST::Identifier::ListPtr identifiers = get_rvalue_identifier_nodes(node);
	return get_property_in_list<std::string, AST::Identifier>
		(identifiers, [](AST::Identifier::Ptr n){return n->get_name();});
}



AST::IODir::ListPtr Module::get_iodir_nodes(AST::Node::Ptr node)
{
	AST::IODir::ListPtr list = std::make_shared<AST::IODir::List>();
	get_node_list_by_category<AST::IODir>(node, AST::NodeType::IODir, list);
	return list;
}

std::vector<std::string> Module::get_iodir_names(AST::Node::Ptr node)
{
	AST::IODir::ListPtr iodirs = get_iodir_nodes(node);
	return get_property_in_list<std::string, AST::IODir>
		(iodirs, [](AST::IODir::Ptr n){return n->get_name();});
}



AST::Output::ListPtr Module::get_output_nodes(AST::Node::Ptr node)
{
	AST::Output::ListPtr list = std::make_shared<AST::Output::List>();
	get_node_list<AST::Output>(node, AST::NodeType::Output, list);
	return list;
}

std::vector<std::string> Module::get_output_names(AST::Node::Ptr node)
{
	AST::Output::ListPtr outputs = get_output_nodes(node);
	return get_property_in_list<std::string, AST::Output>
		(outputs, [](AST::Output::Ptr n){return n->get_name();});
}



AST::Inout::ListPtr Module::get_inout_nodes(AST::Node::Ptr node)
{
	AST::Inout::ListPtr list = std::make_shared<AST::Inout::List>();
	get_node_list<AST::Inout>(node, AST::NodeType::Inout, list);
	return list;
}

std::vector<std::string> Module::get_inout_names(AST::Node::Ptr node)
{
	AST::Inout::ListPtr inouts = get_inout_nodes(node);
	return get_property_in_list<std::string, AST::Inout>
		(inouts, [](AST::Inout::Ptr n){return n->get_name();});
}



AST::Input::ListPtr Module::get_input_nodes(AST::Node::Ptr node)
{
	AST::Input::ListPtr list = std::make_shared<AST::Input::List>();
	get_node_list<AST::Input>(node, AST::NodeType::Input, list);
	return list;
}

std::vector<std::string> Module::get_input_names(AST::Node::Ptr node)
{
	AST::Input::ListPtr inputs = get_input_nodes(node);
	return get_property_in_list<std::string, AST::Input>
		(inputs, [](AST::Input::Ptr n){return n->get_name();});
}



AST::Variable::ListPtr Module::get_variable_nodes(AST::Node::Ptr node)
{
	AST::Variable::ListPtr list = std::make_shared<AST::Variable::List>();
	get_node_list_by_category<AST::Variable>(node, AST::NodeType::Variable, list);
	return list;
}

std::vector<std::string> Module::get_variable_names(AST::Node::Ptr node)
{
	AST::Variable::ListPtr variables = get_variable_nodes(node);
	return get_property_in_list<std::string, AST::Variable>
		(variables, [](AST::Variable::Ptr n){return n->get_name();});
}


AST::Variable::ListPtr Module::get_variable_nodes_within_module(AST::Node::Ptr node)
{
	AST::Variable::ListPtr varlist = get_variable_nodes(node);
	AST::Variable::ListPtr varlist_w = std::make_shared<AST::Variable::List>();
	std::vector<std::string> varname_w = get_variable_names_within_module(node);

	for(AST::Variable::Ptr var: *varlist) {
		const std::string name = var->get_name();

		if(std::find(varname_w.begin(), varname_w.end(), name) != varname_w.end()) {
			varlist_w->push_back(var);
		}
	}

	return varlist_w;
}

std::vector<std::string> Module::get_variable_names_within_module(AST::Node::Ptr node)
{
	std::vector<std::string> var_names = get_variable_names(node);
	std::vector<std::string> io_names = get_iodir_names(node);

	for (const std::string &name: io_names) {
		var_names.erase(std::remove(var_names.begin(), var_names.end(), name), var_names.end());
	}

	return var_names;
}


AST::Instance::ListPtr Module::get_instance_nodes(AST::Node::Ptr node)
{
	AST::Instance::ListPtr list = std::make_shared<AST::Instance::List>();
	get_node_list<AST::Instance>(node, AST::NodeType::Instance, list);
	return list;
}

}
}
}
