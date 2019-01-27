#include <license/license_checker.hpp>

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>

#include <yaml-cpp/yaml.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <ostream>


struct Config
{
	std::vector<std::string> inputs;
	std::string output;
	std::string top_module;
	std::string param_map;
	bool obfuscate {false};
};

static std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "{inputs: [";
	std::stringstream ss;
	for (int i = config.inputs.size()-1; i >= 0; i--) {
		ss << "'" << config.inputs[i] << "'";
		if (i != 0) {
			ss << ", ";
		}
	}
	os << ss.str() << "], ";

	os << "output: '" << config.output << "', ";
	os << "top_module: '" << config.top_module << "', ";
	os << "param_map: " << config.param_map << ", ";
	os << "obfuscate: " << config.obfuscate;
	os << "}";

	return os;
}

static void show_usage(char const * const progname, boost::program_options::options_description const &desc)
{
	boost::filesystem::path p(progname);
	std::cout << "Usage: " << p.filename().string() << " [options] verilog-file [verilog-file ...]"
	          << std::endl;
	std::cout << desc << std::endl;
}

static int veriflat(int argc, char *argv[])
{
	//---------------------------------------------------------
	// Prepare logger
	//---------------------------------------------------------

	Veriparse::Logger::remove_all_sinks();
	Veriparse::Logger::add_text_sink("veriflat.log");
	Veriparse::Logger::add_stdout_sink();


	//---------------------------------------------------------
	// Set seed
	//---------------------------------------------------------

	Veriparse::Passes::Analysis::UniqueDeclaration::seed(0);


	//---------------------------------------------------------
	// Parse command line
	//---------------------------------------------------------

	Config config;

	boost::program_options::options_description options("options");
	options.add_options()
		("help,h", "Produce help message")
		("output,o", boost::program_options::value<std::string>(&config.output)->required(), "output")
		("top-module,t", boost::program_options::value<std::string>(&config.top_module)->required(), "top-module")
		("param-map,p", boost::program_options::value<std::string>(&config.param_map), "YAML parameter map")
		("obfuscate,b", boost::program_options::bool_switch(&config.obfuscate), "obfuscate")
		;

	boost::program_options::options_description hidden("positional");
	hidden.add_options()
		("verilog-file", boost::program_options::value< std::vector<std::string> >(&config.inputs), "verilog file");

	boost::program_options::options_description desc;
	desc.add(options);

	boost::program_options::options_description desc_all;
	desc_all.add(desc).add(hidden);

	boost::program_options::positional_options_description pos;
	pos.add("verilog-file", -1);

	boost::program_options::variables_map vm;
	try {
		boost::program_options::command_line_parser parser(argc, argv);
		auto parsed = parser.options(desc_all).positional(pos).run();
		boost::program_options::store(parsed, vm);
		boost::program_options::notify(vm);
	}
	catch(std::exception& e) {
		LOG_ERROR << e.what();
		show_usage(argv[0], desc);
		return 1;
	}
	catch(...) {
		LOG_ERROR << "Unknown error!";
		show_usage(argv[0], desc);
		return 1;
	}

	if (vm.count("help")) {
		show_usage(argv[0], desc);
		return 1;
	}

	if (vm.count("verilog-file") == 0) {
		LOG_ERROR << "missing verilog file(s)";
		show_usage(argv[0], desc);
	}

	LOG_INFO << "Command line: " << config;


	//---------------------------------------------------------
	// Create a map of all modules
	//---------------------------------------------------------

	Veriparse::Passes::Analysis::Module::ModulesMap modules_map;
	for (const auto &input: config.inputs) {
		Veriparse::Parser::Verilog verilog;
		verilog.parse(input);
		auto source = verilog.get_source();
		Veriparse::Passes::Analysis::Module::get_module_dictionary(source, modules_map);
	}

	if (modules_map.count(config.top_module) == 0) {
		LOG_ERROR << "module " << config.top_module << " not found";
		return 1;
	}

	auto module = modules_map[config.top_module];


	//---------------------------------------------------------
	// Prepare parameter overloading
	//---------------------------------------------------------

	YAML::Node node;

	try {
		node = YAML::Load(config.param_map);
	}
	catch ( const std::exception & e ) {
		LOG_ERROR << "could not read parameters from command line";
		return 1;
	}

	Veriparse::AST::ParamArg::ListPtr paramargs;

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
					return 1;
				}
			}
			ssparams << "endmodule";

			Veriparse::Parser::Verilog verilog;
			verilog.parse(ssparams);
			auto params = Veriparse::Passes::Analysis::Module::get_parameter_nodes(verilog.get_source());

			if (params && params->size() != 0) {
				paramargs = std::make_shared<Veriparse::AST::ParamArg::List>();
				for (auto param: *params) {
					auto name = param->get_name();
					auto rvalue = param->get_value();

					if (rvalue) {
						auto value = Veriparse::AST::cast_to<Veriparse::AST::Rvalue>(rvalue);
						LOG_INFO << "Overloading parameter: " << name << " = "
						         << Veriparse::Generators::VerilogGenerator().render(value);
						paramargs->push_back(std::make_shared<Veriparse::AST::ParamArg>(value, name));
					}
					else {
						LOG_ERROR << "Value of overloaded parameter " << name << " is malformed";
						return 1;
					}
				}
			}
		}
		else {
			LOG_ERROR << "Parameters from the command line are not in a map";
			return 1;
		}
	}


	//---------------------------------------------------------
	// Flatten the selected module
	//---------------------------------------------------------

	Veriparse::Passes::Transformations::ModuleFlattener flattener(paramargs, modules_map);
	flattener.run(module);


	//---------------------------------------------------------
	// Flatten the selected module
	//---------------------------------------------------------

	if (config.obfuscate) {
		LOG_WARNING << "Obfuscator not implemented";
	}


	//---------------------------------------------------------
	// Write the result into the output file
	//---------------------------------------------------------

	const std::string str = Veriparse::Generators::VerilogGenerator().render(module);
	std::ofstream fout(config.output);
	fout << str << std::endl;

	return 0;
}


int main(int argc, char *argv[])
{
	return veriflat(argc, argv);
}
