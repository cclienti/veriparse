#include "parameters_overloading.hpp"

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>

#include <yaml-cpp/yaml.h>

#include <sstream>



Veriparse::AST::ParamArg::ListPtr overload_parameters(const std::string &parameters_string, bool &success)
{
	YAML::Node node;

	try {
		node = YAML::Load(parameters_string);
	}
	catch ( const std::exception & e ) {
		LOG_ERROR << "could not read parameters from command line";
		success = false;
		return nullptr;
	}

	Veriparse::AST::ParamArg::ListPtr param_args;

	if (!node.IsNull()) {
		if (node.IsMap()) {
			std::stringstream ssparams;
			ssparams << "module params;";
			for (auto param: node) {
				if (param.second.IsScalar()) {
					ssparams << "parameter " << param.first << " = " << param.second << ";";
				}
				else {
					LOG_ERROR << "Command line parameter " << param.first << " is not scalar";
					success = false;
					return nullptr;
				}
			}
			ssparams << "endmodule";

			Veriparse::Parser::Verilog verilog;
			verilog.parse(ssparams);
			auto params = Veriparse::Passes::Analysis::Module::get_parameter_nodes(verilog.get_source());

			if (params && params->size() != 0) {
				param_args = std::make_shared<Veriparse::AST::ParamArg::List>();
				for (auto param: *params) {
					auto name = param->get_name();
					auto rvalue = param->get_value();

					if (rvalue) {
						auto value = Veriparse::AST::cast_to<Veriparse::AST::Rvalue>(rvalue);
						LOG_INFO << "Overloading parameter: " << name << " = "
						         << Veriparse::Generators::VerilogGenerator().render(value);
						param_args->push_back(std::make_shared<Veriparse::AST::ParamArg>(value, name));
					}
					else {
						LOG_ERROR << "Value of overloaded parameter " << name << " is malformed";
						success = false;
						return nullptr;
					}
				}
			}
		}
		else {
			LOG_ERROR << "Parameters from the command line are not in a map";
			success = false;
			return nullptr;
		}
	}

	success = true;
	return param_args;
}
