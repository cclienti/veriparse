// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22 and IEEE Std 1364-2005 §19.

#include <parser/preprocessor/pp_driver.hpp>
#include <parser/preprocessor/pp_scanner.hpp>
#include <veriparse/logger/logger.hpp>

#include <filesystem>
#include <fstream>
#include <ostream>

namespace Veriparse
{
namespace Parser
{

int PreprocessorDriver::preprocess(const std::string &filename, std::ostream &out)
{
    std::ifstream in(filename);
    if(!in.good()) {
        LOG_ERROR << "could not open \"" << filename << "\" for reading";
        return 1;
    }
    return run_scanner(in, filename, out);
}

int PreprocessorDriver::preprocess(const std::vector<std::string> &filenames, std::ostream &out)
{
    for(const auto &filename : filenames) {
        if(preprocess(filename, out) != 0) {
            return 1;
        }
    }
    return 0;
}

int PreprocessorDriver::preprocess(std::istream &in, const std::string &filename, std::ostream &out)
{
    if(!in.good()) {
        LOG_ERROR << "input stream for \"" << filename << "\" is not readable";
        return 1;
    }
    return run_scanner(in, filename, out);
}

int PreprocessorDriver::run_scanner(std::istream &in, const std::string &filename,
                                    std::ostream &out)
{
    install_predefines();
    emit_line_marker(out, 1, filename, 0);

    PreprocessorScanner scanner(&in, &out, filename, *this);
    const int rc = scanner.yylex();

    if(!m_conditionals.empty()) {
        LOG_ERROR << "unterminated `ifdef/`ifndef at end of input (opened at \""
                  << m_conditionals.top().opened_filename << "\", line "
                  << m_conditionals.top().opened_line << ")";
        return 1;
    }
    return rc;
}

void PreprocessorDriver::install_predefines()
{
    for(const auto &kv : m_predefines) {
        m_macros.define(kv.first, /*has_args*/ false, /*formals*/ {}, kv.second, "<command-line>",
                        0);
    }
}

std::string PreprocessorDriver::resolve_quoted_include(const std::string &name) const
{
    namespace fs = std::filesystem;

    // §22.4: for quoted form, search CWD then the user-specified -I dirs.
    const fs::path arg(name);
    if(arg.is_absolute()) {
        return fs::exists(arg) ? arg.string() : std::string();
    }
    if(fs::exists(arg)) {
        return arg.string();
    }
    for(const auto &dir : m_include_dirs) {
        const fs::path candidate = fs::path(dir) / arg;
        if(fs::exists(candidate)) {
            return candidate.string();
        }
    }
    return {};
}

void PreprocessorDriver::emit_line_marker(std::ostream &out, int line, const std::string &filename,
                                          int level)
{
    out << "`line " << line << " \"" << filename << "\" " << level << "\n";
}

} // namespace Parser
} // namespace Veriparse
