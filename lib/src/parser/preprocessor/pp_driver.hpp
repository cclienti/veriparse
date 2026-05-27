// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22 and IEEE Std 1364-2005 §19.
#ifndef VERIPARSE_PREPROCESSOR_DRIVER_HPP
#define VERIPARSE_PREPROCESSOR_DRIVER_HPP

#include <parser/preprocessor/pp_conditional.hpp>
#include <parser/preprocessor/pp_macro.hpp>

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Parser
{

class PreprocessorDriver
{
public:
    PreprocessorDriver() = default;
    ~PreprocessorDriver() = default;

    void set_sv_mode(bool sv_mode) { m_sv_mode = sv_mode; }
    bool get_sv_mode() const { return m_sv_mode; }

    void add_include_dir(const std::string &dir) { m_include_dirs.push_back(dir); }

    void define(const std::string &name, const std::string &body) { m_predefines[name] = body; }
    void undef(const std::string &name) { m_predefines.erase(name); }

    int preprocess(const std::string &filename, std::ostream &out);
    int preprocess(const std::vector<std::string> &filenames, std::ostream &out);
    int preprocess(std::istream &in, const std::string &filename, std::ostream &out);

    // --- exposed to the scanner ---

    MacroTable &macros() { return m_macros; }
    ConditionalStack &conditionals() { return m_conditionals; }

    /// Resolve a `include "filename" against the search path. Returns
    /// empty string if not found. (§22.4 quoted form: CWD then -I dirs.)
    std::string resolve_quoted_include(const std::string &name) const;

    /// Emit a `line N "file" level marker (§22.12).
    void emit_line_marker(std::ostream &out, int line, const std::string &filename, int level);

    /// Configured maximum include nesting depth. §22.4 mandates at least
    /// 15; we set a generous default that can be tightened in tests.
    int max_include_depth() const { return 64; }

private:
    int run_scanner(std::istream &in, const std::string &filename, std::ostream &out);
    void install_predefines();

    bool m_sv_mode{false};
    std::vector<std::string> m_include_dirs;
    std::map<std::string, std::string> m_predefines;

    MacroTable m_macros;
    ConditionalStack m_conditionals;
};

} // namespace Parser
} // namespace Veriparse

#endif
