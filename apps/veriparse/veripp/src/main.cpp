// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/version.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

static void show_usage(char const *const progname, po::options_description const &desc)
{
    boost::filesystem::path p(progname);
    std::cout << "Usage: " << p.filename().string() << " [options] verilog-file [verilog-file ...]"
              << std::endl;
    std::cout << desc << std::endl;
}

static int veripp(int argc, char *argv[])
{
    //-----------------------------------------------------------
    // Parse command line
    //-----------------------------------------------------------

    std::string output;
    std::string log_file;
    Veriparse::Parser::PreprocessorOptions opts;
    std::vector<std::string> inputs;

    po::options_description options("options");
    options.add_options()("help,h", "Produce help message")(
        "version,v", "Show the version and exit")("output,o", po::value<std::string>(&output),
                                                  "Output file (default: stdout)")(
        "include-dir,I", po::value<std::vector<std::string>>(&opts.include_dirs),
        "Add directory to `include search path (repeatable)")(
        "define,D", po::value<std::vector<std::string>>(&opts.defines),
        "Predefine a macro as NAME or NAME=BODY (repeatable)")(
        "undef,U", po::value<std::vector<std::string>>(&opts.undefs),
        "Cancel a predefine NAME (repeatable)")("sv", po::bool_switch(&opts.sv_mode),
                                                "Enable SystemVerilog mode")(
        "log", po::value<std::string>(&log_file), "Log to FILE instead of stderr");

    po::options_description hidden("positional");
    hidden.add_options()("verilog-file", po::value<std::vector<std::string>>(&inputs),
                         "verilog file");

    po::options_description desc;
    desc.add(options);

    po::options_description desc_all;
    desc_all.add(desc).add(hidden);

    po::positional_options_description pos;
    pos.add("verilog-file", -1);

    po::variables_map vm;
    try {
        auto parsed = po::command_line_parser(argc, argv).options(desc_all).positional(pos).run();
        po::store(parsed, vm);
        po::notify(vm);
    } catch(std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        show_usage(argv[0], desc);
        return 1;
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

    if(inputs.empty()) {
        std::cerr << "error: missing verilog file" << std::endl;
        show_usage(argv[0], desc);
        return 1;
    }

    //-----------------------------------------------------------
    // Prepare logger — keep stdout clean for the preprocessed stream
    // (emitted to stdout when -o is omitted): diagnostics go to stderr
    // by default, or to FILE when --log FILE is given.
    //-----------------------------------------------------------

    Veriparse::Logger::remove_all_sinks();
    if(!log_file.empty()) {
        Veriparse::Logger::add_text_sink(log_file);
    } else {
        Veriparse::Logger::add_stderr_sink();
    }

    LOG_INFO << "Veriparse version: " << Veriparse::Version::get_version() << " - "
             << Veriparse::Version::get_sha1();

    //-----------------------------------------------------------
    // Run and emit
    //-----------------------------------------------------------

    Veriparse::Parser::Preprocessor pp;
    pp.apply(opts);

    std::ofstream fout;
    std::ostream *out = &std::cout;
    if(!output.empty()) {
        fout.open(output);
        if(!fout) {
            std::cerr << "error: cannot open output file: " << output << std::endl;
            return 1;
        }
        out = &fout;
    }

    return pp.preprocess(inputs, *out);
}

int main(int argc, char *argv[]) { return veripp(argc, argv); }
