#include <license/license_checker.hpp>

#include <veriparse/logger/logger.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>


void show_usage(char const * const progname, boost::program_options::options_description const &desc)
{
	boost::filesystem::path p(progname);
	std::cout << "Usage: " << p.filename().string() << " [options] verilog-file [verilog-file ...]"
	          << std::endl;
	std::cout << desc << std::endl;
}

struct Config
{
	std::vector<std::string> inputs;
	std::string output;
};

static int veriflat(int argc, char *argv[])
{
	Veriparse::Logger::remove_all_sinks();
	Veriparse::Logger::add_text_sink("veriflat.log");
	Veriparse::Logger::add_stdout_sink();

	Config config;

	boost::program_options::options_description options("options");
	options.add_options()
		("help,h", "Produce help message")
		("output,o", boost::program_options::value<std::string>(&config.output)->required(), "output")
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

	return 0;
}


int main(int argc, char *argv[])
{
	return veriflat(argc, argv);
}
