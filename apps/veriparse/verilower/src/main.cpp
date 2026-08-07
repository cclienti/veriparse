// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "config.hpp"

#include <veriparse/logger/logger.hpp>
#include <veriparse/parser/preprocessor.hpp>
#include <veriparse/parser/verilog.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/passes/analysis/module.hpp>
#include <veriparse/passes/analysis/synthesizable_check.hpp>
#include <veriparse/passes/analysis/unique_declaration.hpp>
#include <veriparse/passes/transformations/package_inliner.hpp>
#include <veriparse/passes/transformations/name_resolution.hpp>
#include <veriparse/passes/transformations/default_resolution.hpp>
#include <veriparse/passes/transformations/resolve_module.hpp>
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

static int verilower(int argc, char *argv[])
{
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
        "output")("top-module,t",
                  boost::program_options::value<std::string>(&config.top_module)->required(),
                  "top-module")(
        "suffix", boost::program_options::value<std::string>(&config.suffix)->default_value(""),
        "Append to the emitted module's name, so the output can sit beside "
        "its source in one testbench")("sv", boost::program_options::bool_switch(&config.sv_mode),
                                       "Enable SystemVerilog mode")(
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

    //---------------------------------------------------------
    // Prepare logger: --log FILE captures the log into FILE; with no
    // --log the records go to the console (stderr).
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
    // Per-file compilation units: preprocess + parse each input file on its
    // own (macros do not flow across files).
    //---------------------------------------------------------

    Veriparse::Parser::PreprocessorOptions opts;
    opts.sv_mode = config.sv_mode;
    opts.include_dirs = config.include_dirs;
    opts.defines = config.defines;
    opts.undefs = config.undefs;

    std::vector<Veriparse::AST::Node::Ptr> sources;
    for(const auto &input : config.inputs) {
        std::stringstream preprocessed;
        Veriparse::Parser::Preprocessor pp;
        pp.apply(opts);
        if(pp.preprocess(input, preprocessed) != 0) {
            LOG_ERROR << "preprocessing failed for " << input;
            return 1;
        }
        Veriparse::Parser::Verilog verilog;
        verilog.set_sv_mode(config.sv_mode);
        verilog.parse(preprocessed);
        auto source = verilog.get_source();
        if(!source) {
            LOG_ERROR << "parsing failed for " << input;
            return 1;
        }
        sources.push_back(source);
    }

    //---------------------------------------------------------
    // The design-level preamble veriflat also runs (ADR-0014): packages and
    // imports resolved across units, parser deferrals re-tagged, implicit
    // defaults made explicit — the FSM lowering reads declaration types and
    // directions, so nothing may be left implicit.
    //---------------------------------------------------------

    if(Veriparse::Passes::Transformations::PackageInliner().run_units(sources) != 0) {
        LOG_ERROR << "package/import resolution failed";
        return 1;
    }

    if(Veriparse::Passes::Transformations::NameResolution().run_design(sources) != 0) {
        LOG_ERROR << "name resolution failed";
        return 1;
    }

    if(Veriparse::Passes::Transformations::DefaultResolution(config.sv_mode).run_design(sources) !=
       0) {
        LOG_ERROR << "implicit-default resolution failed";
        return 1;
    }

    //---------------------------------------------------------
    // Select the module to compile.
    //---------------------------------------------------------

    Veriparse::Passes::Analysis::Module::ModulesMap modules_map;
    for(const auto &source : sources) {
        if(Veriparse::Passes::Analysis::Module::get_module_dictionary(source, modules_map) != 0) {
            return 1;
        }
    }

    if(modules_map.count(config.top_module) == 0) {
        LOG_ERROR << "module " << config.top_module << " not found";
        return 1;
    }

    auto module = modules_map[config.top_module];

    //---------------------------------------------------------
    // Resolve the module with the FSM slot enabled (ADR-0014 §10.3): the
    // marked processes compile into explicit machines, and the folding
    // passes clean the result.
    //---------------------------------------------------------

    Veriparse::Passes::Transformations::ResolveModule resolver(Veriparse::AST::ParamArg::ListPtr(),
                                                               modules_map, true, true);
    if(resolver.run(module) != 0) {
        LOG_ERROR << "FSM elaboration failed";
        return 1;
    }

    //---------------------------------------------------------
    // The synthesizable-subset verdict applies to the OUTPUT, never to the
    // input: the input is a process suspending on edge waits, which is
    // precisely what the subset excludes (ADR-0014, ADR-0007 §1.2).
    //---------------------------------------------------------

    std::vector<Veriparse::AST::Node::Ptr> compiled;
    compiled.push_back(module);
    if(Veriparse::Passes::Analysis::SynthesizableCheck::check(compiled) != 0) {
        LOG_ERROR << "compiled output uses non-synthesizable constructs";
        return 1;
    }

    //---------------------------------------------------------
    // Rename on request: the differential cosim of ADR-0014 §11 puts the
    // compiled module beside its own source in one testbench, which needs
    // distinct names.
    //---------------------------------------------------------

    if(!config.suffix.empty()) {
        const auto &module_node = Veriparse::AST::cast_to<Veriparse::AST::Module>(module);
        module_node->set_name(module_node->get_name() + config.suffix);
    }

    //---------------------------------------------------------
    // Write the result into the output file
    //---------------------------------------------------------

    const std::string str = Veriparse::Generators::VerilogGenerator().render(module);
    std::ofstream fout(config.output);
    fout << str << std::endl;

    return 0;
}

int main(int argc, char *argv[]) { return verilower(argc, argv); }
