// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <parser/preprocessor/pp_macro.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Parser
{

void MacroTable::define(const std::string &name, bool has_args,
                        const std::vector<std::string> &formals, const std::string &body,
                        const std::string &filename, int line)
{
    auto it = m_macros.find(name);
    if(it != m_macros.end()) {
        // §22.5.1: "Redefinition of text macros is allowed; the latest
        // definition prevails." Warn so the user notices accidental
        // shadowing.
        LOG_WARNING << "file \"" << filename << "\", line " << line << ": macro '" << name
                    << "' redefined (previous: \"" << it->second.defined_filename << "\", line "
                    << it->second.defined_line << ")";
    }
    Macro m;
    m.name = name;
    m.has_args = has_args;
    m.formals = formals;
    m.body = body;
    m.defined_filename = filename;
    m.defined_line = line;
    m.expansion_depth = 0;
    m_macros[name] = std::move(m);
}

void MacroTable::undef(const std::string &name)
{
    // §22.5.2: undef'ing an undefined macro "can result in a warning".
    auto it = m_macros.find(name);
    if(it == m_macros.end()) {
        LOG_WARNING << "`undef of undefined macro '" << name << "'";
        return;
    }
    m_macros.erase(it);
}

void MacroTable::undef_all()
{
    // §22.5.3 (SV-only): `undefineall drops every macro defined in the
    // current compilation unit.
    m_macros.clear();
}

bool MacroTable::is_defined(const std::string &name) const
{
    return m_macros.find(name) != m_macros.end();
}

Macro *MacroTable::find(const std::string &name)
{
    auto it = m_macros.find(name);
    return it == m_macros.end() ? nullptr : &it->second;
}

} // namespace Parser
} // namespace Veriparse
