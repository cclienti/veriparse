// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "config.hpp"
#include "parameters_overloading.hpp"

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/transformations/module_flattener.hpp>
#include <veriparse/passes/transformations/deadcode_elimination.hpp>
#include <veriparse/passes/transformations/wire_split.hpp>
#include <veriparse/version.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>

#include <string>
#include <cstdint>

static void show_usage(char const *const progname,
                       boost::program_options::options_description const &desc)
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
    options.add_options()("help,h", "Produce help message")("version,v",
                                                            "Show the version and exit")(
        "output,o", boost::program_options::value<std::string>(&config.output)->required(),
        "output")(
        "top-module,t", boost::program_options::value<std::string>(&config.top_module)->required(),
        "top-module")("param-map,p", boost::program_options::value<std::string>(&config.param_map),
                      "YAML parameter map")(
        "deadcode-end,e", boost::program_options::bool_switch(&config.deadcode_at_end),
        "Remove deadcode after flatten pass")(
        "deadcode-during,d", boost::program_options::bool_switch(&config.deadcode_during_flatten),
        "Remove deadcode during flatten pass")(
        "sv", boost::program_options::bool_switch(&config.sv_mode), "Enable SystemVerilog mode")(
        "include-dir,I",
        boost::program_options::value<std::vector<std::string>>(&config.include_dirs),
        "Add directory to `include search path (repeatable)")(
        "define,D", boost::program_options::value<std::vector<std::string>>(&config.defines),
        "Predefine a macro as NAME or NAME=BODY (repeatable)")(
        "undef,U", boost::program_options::value<std::vector<std::string>>(&config.undefs),
        "Cancel a predefine NAME (repeatable)")(
        "seed,s", boost::program_options::value<std::uint64_t>(&config.seed)->default_value(0),
        "Seed value");

    boost::program_options::options_description hidden("positional");
    hidden.add_options()("verilog-file",
                         boost::program_options::value<std::vector<std::string>>(&config.inputs),
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
    // Preprocess all inputs into a single in-memory stream.
    // Backward-compatible: input that has already been processed
    // by another preprocessor (e.g. iverilog -E) round-trips
    // through veripp untouched.
    //---------------------------------------------------------

    std::stringstream preprocessed;
    {
        Veriparse::Parser::Preprocessor pp;
        pp.set_sv_mode(config.sv_mode);
        for(const auto &dir : config.include_dirs) {
            pp.add_include_dir(dir);
        }
        for(const auto &spec : config.defines) {
            const auto eq = spec.find('=');
            if(eq == std::string::npos) {
                pp.define(spec);
            } else {
                pp.define(spec.substr(0, eq), spec.substr(eq + 1));
            }
        }
        for(const auto &name : config.undefs) {
            pp.undef(name);
        }
        if(pp.preprocess(config.inputs, preprocessed) != 0) {
            LOG_ERROR << "preprocessing failed";
            return 1;
        }
    }

    //---------------------------------------------------------
    // Create a map of all modules
    //---------------------------------------------------------

    Veriparse::Passes::Analysis::Module::ModulesMap modules_map;

    {
        Veriparse::Parser::Verilog verilog;
        verilog.set_sv_mode(config.sv_mode);
        verilog.parse(preprocessed);
        auto source = verilog.get_source();
        Veriparse::Passes::Analysis::Module::get_module_dictionary(source, modules_map);
    }

    if(modules_map.count(config.top_module) == 0) {
        LOG_ERROR << "module " << config.top_module << " not found";
        return 1;
    }

    auto module = modules_map[config.top_module];

    //---------------------------------------------------------
    // Prepare parameter overloading
    //---------------------------------------------------------

    bool overloaded;
    Veriparse::AST::ParamArg::ListPtr param_args =
        overload_parameters(config.param_map, overloaded);
    if(!overloaded) {
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

    if(config.deadcode_at_end) {
        Veriparse::Passes::Transformations::DeadcodeElimination deadcode_elim;
        deadcode_elim.run(module);
    }

    //---------------------------------------------------------

    //---------------------------------------------------------
    // Wire split: separate inline wire declarations from assignments
    //---------------------------------------------------------

    {
        Veriparse::Passes::Transformations::WireSplit wire_split;
        wire_split.run(module);
    }

    // Write the result into the output file
    //---------------------------------------------------------

    const std::string str = Veriparse::Generators::VerilogGenerator().render(module);
    std::ofstream fout(config.output);
    fout << str << std::endl;

    return 0;
}

int main(int argc, char *argv[]) { return veriflat(argc, argv); }
