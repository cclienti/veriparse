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
    bool sv_mode = false;
    std::vector<std::string> include_dirs;
    std::vector<std::string> defines;
    std::vector<std::string> undefs;
    std::vector<std::string> inputs;

    po::options_description options("options");
    options.add_options()("help,h", "Produce help message")(
        "version,v", "Show the version and exit")("output,o", po::value<std::string>(&output),
                                                  "Output file (default: stdout)")(
        "include-dir,I", po::value<std::vector<std::string>>(&include_dirs),
        "Add directory to `include search path (repeatable)")(
        "define,D", po::value<std::vector<std::string>>(&defines),
        "Predefine a macro as NAME or NAME=BODY (repeatable)")(
        "undef,U", po::value<std::vector<std::string>>(&undefs),
        "Cancel a predefine NAME (repeatable)")("sv", po::bool_switch(&sv_mode),
                                                "Enable SystemVerilog mode");

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
    // Prepare logger — keep stdout clean for the preprocessed
    // stream when -o is omitted; route diagnostics to veripp.log
    // only.
    //-----------------------------------------------------------

    Veriparse::Logger::remove_all_sinks();
    Veriparse::Logger::add_text_sink("veripp.log");

    LOG_INFO << "Veriparse version: " << Veriparse::Version::get_version() << " - "
             << Veriparse::Version::get_sha1();

    //-----------------------------------------------------------
    // Configure the preprocessor
    //-----------------------------------------------------------

    Veriparse::Parser::Preprocessor pp;
    pp.set_sv_mode(sv_mode);
    for(const auto &dir : include_dirs) {
        pp.add_include_dir(dir);
    }
    for(const auto &spec : defines) {
        const auto eq = spec.find('=');
        if(eq == std::string::npos) {
            pp.define(spec);
        } else {
            pp.define(spec.substr(0, eq), spec.substr(eq + 1));
        }
    }
    for(const auto &name : undefs) {
        pp.undef(name);
    }

    //-----------------------------------------------------------
    // Run and emit
    //-----------------------------------------------------------

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
