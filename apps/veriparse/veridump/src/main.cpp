// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/version.hpp>
#include <yaml-cpp/yaml.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static void show_usage(char const *const progname,
                       boost::program_options::options_description const &desc)
{
    boost::filesystem::path p(progname);
    std::cout << "Usage: " << p.filename().string() << " [options] verilog-file [verilog-file ...]"
              << std::endl;
    std::cout << desc << std::endl;
}

static int veridump(int argc, char *argv[])
{
    //-----------------------------------------------------------
    // Prepare logger
    //-----------------------------------------------------------

    Veriparse::Logger::remove_all_sinks();
    Veriparse::Logger::add_text_sink("veridump.log");
    Veriparse::Logger::add_stdout_sink();

    //-----------------------------------------------------------
    // Parse command line
    //-----------------------------------------------------------

    std::string output;
    std::string format;
    bool sv_mode = false;
    std::vector<std::string> inputs;
    std::vector<std::string> include_dirs;
    std::vector<std::string> defines;
    std::vector<std::string> undefs;

    boost::program_options::options_description options("options");
    options.add_options()("help,h", "Produce help message")("version,v",
                                                            "Show the version and exit")(
        "output,o", boost::program_options::value<std::string>(&output)->required(), "Output file")(
        "format,f", boost::program_options::value<std::string>(&format)->default_value("yaml"),
        "Output format: yaml or dot")("sv", boost::program_options::bool_switch(&sv_mode),
                                      "Enable SystemVerilog mode")(
        "include-dir,I", boost::program_options::value<std::vector<std::string>>(&include_dirs),
        "Add directory to `include search path (repeatable)")(
        "define,D", boost::program_options::value<std::vector<std::string>>(&defines),
        "Predefine a macro as NAME or NAME=BODY (repeatable)")(
        "undef,U", boost::program_options::value<std::vector<std::string>>(&undefs),
        "Cancel a predefine NAME (repeatable)");

    boost::program_options::options_description hidden("positional");
    hidden.add_options()("verilog-file",
                         boost::program_options::value<std::vector<std::string>>(&inputs),
                         "verilog file");

    boost::program_options::options_description desc;
    desc.add(options);

    boost::program_options::options_description desc_all;
    desc_all.add(desc).add(hidden);

    boost::program_options::positional_options_description pos;
    pos.add("verilog-file", -1);

    boost::program_options::variables_map vm;
    auto parsed = boost::program_options::command_line_parser(argc, argv)
                      .options(desc_all)
                      .positional(pos)
                      .run();
    boost::program_options::store(parsed, vm);

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
    }

    if(format != "yaml" && format != "dot") {
        LOG_ERROR << "unknown format '" << format << "': use 'yaml' or 'dot'";
        return 1;
    }

    if(inputs.empty()) {
        LOG_ERROR << "missing verilog file";
        show_usage(argv[0], desc);
        return 1;
    }

    LOG_INFO << "Veriparse version: " << Veriparse::Version::get_version() << " - "
             << Veriparse::Version::get_sha1();

    //-----------------------------------------------------------
    // Preprocess all inputs into a single in-memory stream.
    // Backward-compatible: input that has already been processed
    // by another preprocessor (e.g. iverilog -E) round-trips
    // through veripp untouched.
    //-----------------------------------------------------------

    std::stringstream preprocessed;
    {
        Veriparse::Parser::PreprocessorOptions opts;
        opts.sv_mode = sv_mode;
        opts.include_dirs = include_dirs;
        opts.defines = defines;
        opts.undefs = undefs;
        Veriparse::Parser::Preprocessor pp;
        pp.apply(opts);
        if(pp.preprocess(inputs, preprocessed) != 0) {
            LOG_ERROR << "preprocessing failed";
            return 1;
        }
    }

    //-----------------------------------------------------------
    // Parse all input files into a single AST
    //-----------------------------------------------------------

    Veriparse::Parser::Verilog verilog;
    verilog.set_sv_mode(sv_mode);
    verilog.parse(preprocessed);

    auto source = verilog.get_source();
    if(!source) {
        LOG_ERROR << "failed to parse input";
        return 1;
    }

    //-----------------------------------------------------------
    // Render and write output
    //-----------------------------------------------------------

    std::string result;

    if(format == "yaml") {
        YAML::Emitter emitter;
        emitter << Veriparse::Generators::YAMLGenerator().render(source);
        result = emitter.c_str();
    } else {
        // dot: use the AST tree dot method
        // dot: use save_dot to get the proper digraph wrapper
        Veriparse::Generators::DotGenerator::save_dot(
            Veriparse::Generators::DotGenerator().render(source), output);
        return 0;
    }

    std::ofstream fout(output);
    if(!fout) {
        LOG_ERROR << "cannot open output file: " << output;
        return 1;
    }
    fout << result << "\n";

    return 0;
}

int main(int argc, char *argv[]) { return veridump(argc, argv); }
