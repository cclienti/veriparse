#include "config.hpp"

#include <license/license_checker.hpp>

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/transformations/module_obfuscator.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>


static void show_usage(char const * const progname, boost::program_options::options_description const &desc)
{
	boost::filesystem::path p(progname);
	std::cout << "Usage: " << p.filename().string() << " [options] verilog-file"
	          << std::endl;
	std::cout << desc << std::endl;
}


static int veriobf(int argc, char *argv[])
{
	//---------------------------------------------------------
	// Prepare logger
	//---------------------------------------------------------

	Veriparse::Logger::remove_all_sinks();
	Veriparse::Logger::add_text_sink("veriobf.log");
	Veriparse::Logger::add_stdout_sink();


	//---------------------------------------------------------
	// Parse command line
	//---------------------------------------------------------

	Config config;

	boost::program_options::options_description options("options");
	options.add_options()
		("help,h", "Produce help message")
		("output,o", boost::program_options::value<std::string>(&config.output)->required(), "Output file")
		("id-length,l", boost::program_options::value<std::uint64_t>(&config.identifier_length)->default_value(16),
		 "Maximum length of obfuscated indentifiers")
		("hash,a", boost::program_options::bool_switch(&config.hash)->default_value(true),
		 "Use hashed identifiers instead of random ones")
		("seed,s", boost::program_options::value<std::uint64_t>(&config.seed)->default_value(0), "Seed value");

	boost::program_options::options_description hidden("positional");
	hidden.add_options()
		("verilog-file", boost::program_options::value<std::string> (&config.input), "verilog file");

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
		LOG_ERROR << "missing verilog file";
		show_usage(argv[0], desc);
	}

	LOG_INFO << "Command line: " << config;


	//---------------------------------------------------------
	// Set seed
	//---------------------------------------------------------

	Veriparse::Passes::Analysis::UniqueDeclaration::seed(config.seed);


	//---------------------------------------------------------
	// Create a map of all modules
	//---------------------------------------------------------

	Veriparse::Passes::Analysis::Module::ModulesMap modules_map;

	Veriparse::Parser::Verilog verilog;
	verilog.parse(config.input);
	auto source = verilog.get_source();
	Veriparse::Passes::Analysis::Module::get_module_dictionary(source, modules_map);


	//---------------------------------------------------------
	// Check license
	//---------------------------------------------------------

	if (license::check_license()) {
		return 1;
	}


	//---------------------------------------------------------
	// Flatten the selected module
	//---------------------------------------------------------

	for (const auto &module: modules_map) {
		Veriparse::Passes::Transformations::ModuleObfuscator obfuscator(config.identifier_length, config.hash);
		obfuscator.run(module.second);
	}


	//---------------------------------------------------------
	// Write the result into the output file
	//---------------------------------------------------------

	std::ofstream fout(config.output);

	for (const auto &module: modules_map) {
		const std::string str = Veriparse::Generators::VerilogGenerator().render(module.second);
		fout << str << std::endl;
	}

	return 0;
}


int main(int argc, char *argv[])
{
	return veriobf(argc, argv);
}
