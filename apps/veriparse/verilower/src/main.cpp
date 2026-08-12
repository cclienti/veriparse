// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include "config.hpp"
#include "report.hpp"
#include "parameters_overloading.hpp"
#include "fsm_mark.hpp"

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
        "output")(
        "top-module,t", boost::program_options::value<std::string>(&config.top_module)->required(),
        "top-module")("param-map,p", boost::program_options::value<std::string>(&config.param_map),
                      "YAML parameter map for the top module: {N: 42} overrides, {N:} keeps")(
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
                      "Log to FILE instead of stderr")(
        "state-map", boost::program_options::value<std::string>(&config.state_map),
        "Write the ADR-0014 state map (JSON) to FILE; default <output>.fsm.json")(
        "fsm-dot", boost::program_options::value<std::string>(&config.fsm_dot),
        "Write a graphviz view of the compiled machines to FILE")(
        "fsm-dot-values", boost::program_options::bool_switch(&config.fsm_dot_values),
        "Label the graphviz edges with the register updates too");

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
    // A marked process outside the selected module is not compiled, and the
    // mark is never skipped in silence (ADR-0014 §2): each marked module is
    // its own run, parameterized by --param-map or its defaults — an
    // instantiation with parameter overrides inside the design needs a
    // per-instance flow instead.
    //---------------------------------------------------------

    for(const auto &elt : modules_map) {
        if(elt.first == config.top_module) {
            continue;
        }
        if(has_veriparse_fsm_mark(elt.second)) {
            LOG_WARNING << "module '" << elt.first << "' carries (* veriparse_fsm *) "
                        << "processes but only '" << config.top_module << "' is compiled: "
                        << "run verilower with --top-module " << elt.first
                        << " as its own step, or veriflat --fsm to compile every "
                        << "instantiation";
        }
    }

    //---------------------------------------------------------
    // Parameterize the top from the command line: {N: 42} overrides the
    // default, {N:} keeps N a parameter of the output. A parameter left
    // symbolic that the machine's structure depends on resolves through
    // the non-constant paths (§7.2 counts, §7.3 conditions) or fails
    // loudly — never a silently different machine.
    //---------------------------------------------------------

    bool overloaded;
    Veriparse::AST::ParamArg::ListPtr param_args =
        overload_parameters(config.param_map, overloaded);
    if(!overloaded) {
        return 1;
    }

    //---------------------------------------------------------
    // Resolve the module with the FSM slot enabled (ADR-0014 §10.3): the
    // marked processes compile into explicit machines, and the folding
    // passes clean the result.
    //---------------------------------------------------------

    Veriparse::Passes::Transformations::ImplicitFsmElaboration::FsmReport fsm_report;
    Veriparse::Passes::Transformations::ResolveModule resolver(param_args, modules_map, true, true,
                                                               &fsm_report);
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

    //---------------------------------------------------------
    // The §10.2 state map: JSON is the canonical record of what the source
    // does not state — encoding, naming, the reset contract. The graphviz
    // view is printed from the same report: states as circles, the reset
    // entry as a double circle, guards on the edges, the register updates
    // only on request.
    //---------------------------------------------------------

    if(!fsm_report.processes.empty() || !config.state_map.empty()) {
        const std::string map_path =
            config.state_map.empty() ? config.output + ".fsm.json" : config.state_map;
        std::ofstream json(map_path);
        json << render_state_map(fsm_report);
        LOG_INFO << "state map written to " << map_path;
    }
    if(!config.fsm_dot.empty()) {
        std::ofstream dot(config.fsm_dot);
        dot << render_fsm_dot(fsm_report, config.fsm_dot_values);
        LOG_INFO << "graphviz view written to " << config.fsm_dot;
    }

    return 0;
}

int main(int argc, char *argv[]) { return verilower(argc, argv); }
