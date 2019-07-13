#include "config.hpp"
#include "parameters_overloading.hpp"

#include <license/license_checker.hpp>

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/deadcode_elimination.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <cstdint>


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
		("deadcode-end,e", boost::program_options::bool_switch(&config.deadcode_at_end),
		 "Remove deadcode after flatten pass")
		("deadcode-during,d", boost::program_options::bool_switch(&config.deadcode_during_flatten),
		 "Remove deadcode during flatten pass")
		("seed,s", boost::program_options::value<std::uint64_t>(&config.seed)->default_value(0), "Seed value")
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

	bool overloaded;
	Veriparse::AST::ParamArg::ListPtr param_args = overload_parameters(config.param_map, overloaded);
	if (!overloaded) {
		return 1;
	}


	//---------------------------------------------------------
	// Check license
	//---------------------------------------------------------

	if (license::check_license()) {
		return 1;
	}


	//---------------------------------------------------------
	// Flatten the selected module
	//---------------------------------------------------------

	Veriparse::Passes::Transformations::ModuleFlattener flattener(param_args, modules_map,
	                                                              config.deadcode_during_flatten);
	flattener.run(module);


	//---------------------------------------------------------
	// Extra deadcode elimination pass
	//---------------------------------------------------------

	if (config.deadcode_at_end) {
		Veriparse::Passes::Transformations::DeadcodeElimination deadcode_elim;
		deadcode_elim.run(module);
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
