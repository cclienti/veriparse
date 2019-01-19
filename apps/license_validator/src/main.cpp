#include <license/platform_id.hpp>
#include <license/public_key.hpp>
#include <license/license_checker.hpp>
#include <veriparse/logger/logger.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


void show_usage(char const * const progname, boost::program_options::options_description const &desc)
{
	boost::filesystem::path p(progname);
	std::cout << "Usage: " << p.filename().string() << " [options] <license-file>" << std::endl;
	std::cout << desc << std::endl;
}

int main(int argc, char *argv[])
{
	std::string config_file;
	std::string private_file;

	Veriparse::Logger::remove_all_sinks();
	Veriparse::Logger::add_text_sink("license_validator.log");
	Veriparse::Logger::add_stdout_sink();

	boost::program_options::options_description options("options");
	options.add_options()
		("help,h", "Produce help message");

	boost::program_options::options_description hidden("positional");
	hidden.add_options()
		("license-file", boost::program_options::value<std::string>(), "license file");

	boost::program_options::options_description desc;

	boost::program_options::options_description desc_all;
	desc_all.add(desc).add(hidden);

	boost::program_options::positional_options_description pos;
	pos.add("license-file", 1);

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

	if(!vm.count("license-file")) {
	    LOG_ERROR << "No license file given";
	    return 1;
	}

	auto license_file = vm["license-file"].as<std::string>();

	int rc = license::check_license(license_file);
	if (rc != 0) {
		LOG_ERROR << "license validation failed: " << license_file;
		return 1;
	}

	LOG_INFO << "license file " << license_file << " is valid";
	return 0;
}
