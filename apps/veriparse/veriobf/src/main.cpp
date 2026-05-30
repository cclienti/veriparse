// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "config.hpp"

#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/transformations/module_obfuscator.hpp>
#include <veriparse/version.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <sstream>

#include <string>

static void show_usage(char const *const progname,
                       boost::program_options::options_description const &desc)
{
    boost::filesystem::path p(progname);
    std::cout << "Usage: " << p.filename().string() << " [options] verilog-file" << std::endl;
    std::cout << desc << std::endl;
}

static int veriobf(int argc, char *argv[])
{
    //---------------------------------------------------------
    // Parse command line
    //---------------------------------------------------------

    Config config;

    boost::program_options::options_description options("options");
    options.add_options()("help,h", "Produce help message")("version,v",
                                                            "Show the version and exit")(
        "output,o", boost::program_options::value<std::string>(&config.output)->required(),
        "Output file")(
        "id-length,l",
        boost::program_options::value<std::uint64_t>(&config.identifier_length)->default_value(16),
        "Maximum length of obfuscated indentifiers")(
        "hash,a", boost::program_options::bool_switch(&config.hash),
        "Use hashed identifiers instead of random ones")(
        "sv", boost::program_options::bool_switch(&config.sv_mode), "Enable SystemVerilog mode")(
        "include-dir,I",
        boost::program_options::value<std::vector<std::string>>(&config.include_dirs),
        "Add directory to `include search path (repeatable)")(
        "define,D", boost::program_options::value<std::vector<std::string>>(&config.defines),
        "Predefine a macro as NAME or NAME=BODY (repeatable)")(
        "undef,U", boost::program_options::value<std::vector<std::string>>(&config.undefs),
        "Cancel a predefine NAME (repeatable)")(
        "seed,s", boost::program_options::value<std::uint64_t>(&config.seed)->default_value(0),
        "Seed value")("log", boost::program_options::value<std::string>(&config.log_file),
                      "Log to FILE instead of stderr");

    boost::program_options::options_description hidden("positional");
    hidden.add_options()("verilog-file", boost::program_options::value<std::string>(&config.input),
                         "verilog file");

    boost::program_options::options_description desc;
    desc.add(options);

    boost::program_options::options_description desc_all;
    desc_all.add(desc).add(hidden);

    boost::program_options::positional_options_description pos;
    pos.add("verilog-file", -1);

    boost::program_options::variables_map vm;

    boost::program_options::command_line_parser parser(argc, argv);
    auto parsed = parser.options(desc_all).positional(pos).run();
    boost::program_options::store(parsed, vm);

    //---------------------------------------------------------
    // Prepare logger: --log FILE captures the log into FILE; otherwise
    // records go to stderr. Read vm directly so logging is ready before
    // the notify() below can raise.
    //---------------------------------------------------------

    Veriparse::Logger::remove_all_sinks();
    if(vm.count("log")) {
        Veriparse::Logger::add_text_sink(vm["log"].as<std::string>());
    } else {
        Veriparse::Logger::add_stderr_sink();
    }

    if(vm.count("help")) {
        show_usage(argv[0], desc);
        return 0;
    }

    if(vm.count("version")) {
        std::cout << Veriparse::Version::get_version() << "\n"
                  << Veriparse::Version::get_sha1() << std::endl;
        return 0;
    }

    try {
        boost::program_options::notify(vm);
    } catch(std::exception &e) {
        LOG_ERROR << e.what();
        show_usage(argv[0], desc);
        return 1;
    } catch(...) {
        LOG_ERROR << "Unknown error!";
        show_usage(argv[0], desc);
        return 1;
    }

    if(vm.count("verilog-file") == 0) {
        LOG_ERROR << "missing verilog file";
        show_usage(argv[0], desc);
        return 1;
    }

    LOG_INFO << "Veriparse version: " << Veriparse::Version::get_version() << " - "
             << Veriparse::Version::get_sha1();

    LOG_INFO << "Command line: " << config;

    //---------------------------------------------------------
    // Set seed
    //---------------------------------------------------------

    Veriparse::Passes::Analysis::UniqueDeclaration::seed(config.seed);

    //---------------------------------------------------------
    // Preprocess the input into an in-memory stream.
    // Backward-compatible: input that has already been processed
    // by another preprocessor (e.g. iverilog -E) round-trips
    // through veripp untouched.
    //---------------------------------------------------------

    std::stringstream preprocessed;
    {
        Veriparse::Parser::PreprocessorOptions opts;
        opts.sv_mode = config.sv_mode;
        opts.include_dirs = config.include_dirs;
        opts.defines = config.defines;
        opts.undefs = config.undefs;
        Veriparse::Parser::Preprocessor pp;
        pp.apply(opts);
        if(pp.preprocess(config.input, preprocessed) != 0) {
            LOG_ERROR << "preprocessing failed";
            return 1;
        }
    }

    //---------------------------------------------------------
    // Create a map of all modules
    //---------------------------------------------------------

    Veriparse::Passes::Analysis::Module::ModulesMap modules_map;

    Veriparse::Parser::Verilog verilog;
    verilog.set_sv_mode(config.sv_mode);
    verilog.parse(preprocessed);
    auto source = verilog.get_source();
    Veriparse::Passes::Analysis::Module::get_module_dictionary(source, modules_map);

    //---------------------------------------------------------
    // Obfuscate all modules
    //---------------------------------------------------------

    for(const auto &module : modules_map) {
        Veriparse::Passes::Transformations::ModuleObfuscator obfuscator(config.identifier_length,
                                                                        config.hash);
        obfuscator.run(module.second);
    }

    //---------------------------------------------------------
    // Write the result into the output file
    //---------------------------------------------------------

    std::ofstream fout(config.output);

    for(const auto &module : modules_map) {
        const std::string str = Veriparse::Generators::VerilogGenerator().render(module.second);
        fout << str << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[]) { return veriobf(argc, argv); }
