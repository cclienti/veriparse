// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22.5 and IEEE Std 1364-2005 §19.3.
#ifndef VERIPARSE_PREPROCESSOR_MACRO_HPP
#define VERIPARSE_PREPROCESSOR_MACRO_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace Veriparse
{
namespace Parser
{

struct Macro
{
    std::string name;
    /// True for function-like macros declared as `define X(...). Object-
    /// like macros use formals empty and has_args=false. The distinction
    /// matters at use time (§22.5.1): function-like usage REQUIRES
    /// parens; object-like must NOT consume following parens.
    bool has_args{false};
    std::vector<std::string> formals;
    std::string body;
    std::string defined_filename;
    int defined_line{0};
    /// Per-macro expansion depth (§22.5.1 recursion rule). We use a
    /// counter rather than a boolean because the spec example
    /// `TOP(`TOP(...), `TOP(...)) is required to work — the same macro
    /// can legitimately appear several times on the active stack as
    /// long as the chain terminates. Excessive depth means infinite
    /// recursion and triggers an error.
    int expansion_depth{0};
};

class MacroTable
{
public:
    void define(const std::string &name, bool has_args, const std::vector<std::string> &formals,
                const std::string &body, const std::string &filename, int line);
    void undef(const std::string &name);
    void undef_all();

    bool is_defined(const std::string &name) const;
    Macro *find(const std::string &name);

private:
    std::unordered_map<std::string, Macro> m_macros;
};

} // namespace Parser
} // namespace Veriparse

#endif
