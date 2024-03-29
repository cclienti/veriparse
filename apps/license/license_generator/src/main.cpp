#include "license_generator.hpp"
#include "license_generator.hpp"

#include <license/platform_id.hpp>
#include <license/public_key.hpp>
#include <veriparse/logger/logger.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


int read_file(std::string const &filename, std::string &str) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		str.clear();
		in.seekg(0, std::ios::end);
		str.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&str[0], str.size());
		in.close();
		return 0;
	} else {
		LOG_ERROR << "Could not read the file " << filename;
		return -1;
	}
}

void show_usage(char const * const progname, boost::program_options::options_description const &desc)
{
	boost::filesystem::path p(progname);
	std::cout << "Usage: " << p.filename().string()
	          << " [options] --private <key-file> <license-file>"
	          << std::endl;
	std::cout << desc << std::endl;
}

int main(int argc, char *argv[])
{
	std::string config_file;
	std::string private_file;

	Veriparse::Logger::remove_all_sinks();
	Veriparse::Logger::add_text_sink("license_generator.log");
	Veriparse::Logger::add_stdout_sink();

	boost::program_options::options_description options("options");
	options.add_options()
		("help,h", "Produce help message");

	boost::program_options::options_description key_params("key parameters");
	key_params.add_options()
		("private", boost::program_options::value<std::string>(&private_file)->required(),
		 "Private encryption key file");

	boost::program_options::options_description hidden("positional");
	hidden.add_options()
		("license-file", boost::program_options::value<std::string>(), "license file");

	boost::program_options::options_description desc;
	desc.add(options).add(key_params);

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

	std::string rsa_private_key;
	if (read_file(private_file, rsa_private_key) != 0) return 1;

	char *c_public_key = license::create_public_key();
	std::string rsa_public_key(c_public_key);
	license::shred_string(c_public_key);

	int rc = generate_license(rsa_private_key, rsa_public_key, license_file);
	rsa_public_key.erase(rsa_public_key.begin(), rsa_public_key.end());

	if (rc != 0) return 1;

	return 0;
}
