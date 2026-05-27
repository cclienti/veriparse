// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Method bodies for PreprocessorScanner. The Flex-generated yylex lives
// in pp_scanner.cpp (built from pp_scanner.ll); everything else is here
// so the .ll stays focused on lexing.
//
// Derived from IEEE Std 1800-2017 §22 and IEEE Std 1364-2005 §19.

#include <parser/preprocessor/pp_scanner.hpp>
#include <parser/preprocessor/pp_driver.hpp>
#include <veriparse/logger/logger.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>

namespace Veriparse
{
namespace Parser
{

namespace
{

const std::set<std::string> &pass_through_directives()
{
    // Directives the preprocessor does not act on; emit verbatim and let
    // the main parser/scanner handle them. (§§19.1, 19.2, 19.6–19.10,
    // 22.7–22.12 — `timescale, `default_nettype, `line are consumed by
    // the main scanner today.) `resetall is handled separately because
    // per §22.3 it MUST NOT touch the macro table.
    static const std::set<std::string> set{
        "timescale",         "default_nettype",     "line",   "celldefine", "endcelldefine",
        "unconnected_drive", "nounconnected_drive", "pragma",
    };
    return set;
}

/// Maximum per-macro expansion depth. The spec example
/// `TOP(`TOP(...), `TOP(...)) demands depth >= 2; anything deeper than
/// this on the same macro almost certainly indicates infinite recursion.
constexpr int kMaxExpansionDepth = 64;

/// Textual processing of a macro body before it gets pushed back into
/// the input stream:
///   - identifier-aware substitution of formal arguments (§22.5.1)
///   - SV-only body escapes:
///       `"        emits a plain "  (synthetic string delimiter, §22.5.1)
///       `\`"      emits \"          (escaped quote inside a synthetic string)
///       ``        emits nothing     (token paste, no whitespace introduced)
///
/// Regular string literals "..." inside the body are copied verbatim
/// per §22.5.1 ("Macro substitution and argument substitution shall not
/// occur within string literals.") so a formal whose name happens to
/// appear inside a "..." is not substituted.
///
/// Known limitation: a nested macro reference appearing inside a
/// `" ... `" synthetic string is not re-expanded after substitution.
/// The spec allows that case; we defer it as a corner-case.
std::string expand_body(const Macro &macro, const std::vector<std::string> &actuals, bool sv_mode)
{
    const std::string &body = macro.body;
    const std::vector<Formal> &formals = macro.formals;
    const bool has_formals = macro.has_args && !formals.empty();

    std::string out;
    out.reserve(body.size() * 2);

    size_t i = 0;
    while(i < body.size()) {
        const char c = body[i];

        // SV-only body escapes (§22.5.1). Checked before regular-string
        // handling so a leading `" opens a synthetic-string region
        // rather than being treated as a stray backtick + " sequence.
        if(sv_mode && c == '`' && i + 1 < body.size()) {
            // `\`" — 4 chars: ` \ ` " → emit \"
            if(body[i + 1] == '\\' && i + 3 < body.size() && body[i + 2] == '`' &&
               body[i + 3] == '"') {
                out += "\\\"";
                i += 4;
                continue;
            }
            // `" — emit a plain quote (synthetic string delimiter).
            if(body[i + 1] == '"') {
                out += '"';
                i += 2;
                continue;
            }
            // `` — token paste, no character emitted.
            if(body[i + 1] == '`') {
                i += 2;
                continue;
            }
        }

        // Regular string literal — copy verbatim; no substitution inside.
        if(c == '"') {
            out += c;
            ++i;
            while(i < body.size()) {
                const char d = body[i];
                out += d;
                ++i;
                if(d == '\\' && i < body.size()) {
                    out += body[i];
                    ++i;
                } else if(d == '"') {
                    break;
                }
            }
            continue;
        }

        // Escaped identifier `\foo` extends to whitespace — copy as is.
        if(c == '\\') {
            out += c;
            ++i;
            while(i < body.size() && !std::isspace(static_cast<unsigned char>(body[i]))) {
                out += body[i];
                ++i;
            }
            continue;
        }

        // Identifier — candidate for formal substitution.
        const auto isid_start = [](unsigned char ch) { return std::isalpha(ch) || ch == '_'; };
        const auto isid_cont = [](unsigned char ch) {
            return std::isalnum(ch) || ch == '_' || ch == '$';
        };
        if(isid_start(static_cast<unsigned char>(c))) {
            const size_t start = i;
            while(i < body.size() && isid_cont(static_cast<unsigned char>(body[i]))) {
                ++i;
            }
            const std::string ident = body.substr(start, i - start);
            if(has_formals) {
                const auto it = std::find_if(formals.begin(), formals.end(),
                                             [&](const Formal &f) { return f.name == ident; });
                if(it != formals.end()) {
                    out += actuals[static_cast<size_t>(it - formals.begin())];
                    continue;
                }
            }
            out += ident;
            continue;
        }

        out += c;
        ++i;
    }
    return out;
}

} // namespace

PreprocessorScanner::PreprocessorScanner(std::istream *in, std::ostream *out,
                                         const std::string &filename, PreprocessorDriver &driver)
    : yyFlexLexer(in, out), m_driver(driver), m_output(out), m_filename(filename)
{
}

PreprocessorScanner::~PreprocessorScanner()
{
    while(!m_frames.empty()) {
        m_frames.pop_back();
    }
}

bool PreprocessorScanner::emitting() const { return m_driver.conditionals().emitting(); }

void PreprocessorScanner::emit(const char *text)
{
    if(emitting()) {
        LexerOutput(text, static_cast<int>(std::char_traits<char>::length(text)));
    }
}

void PreprocessorScanner::emit(const std::string &text)
{
    if(emitting()) {
        LexerOutput(text.data(), static_cast<int>(text.size()));
    }
}

void PreprocessorScanner::handle_backtick(const char *name_after_tick)
{
    const std::string name(name_after_tick);

    // Conditional directives are recognised unconditionally — they
    // drive the emit/skip state machine.
    if(name == "ifdef") {
        cond_set_kind_ifdef();
        begin_cond_name();
        return;
    }
    if(name == "ifndef") {
        cond_set_kind_ifndef();
        begin_cond_name();
        return;
    }
    if(name == "elsif") {
        cond_set_kind_elsif();
        begin_cond_name();
        return;
    }
    if(name == "else") {
        cond_apply_else();
        return;
    }
    if(name == "endif") {
        cond_apply_endif();
        return;
    }

    // In a skipped region, swallow the rest of the directive line.
    // §22.6 says ignored lines must remain lex-valid; we discard them
    // here without applying their effects (e.g. a `define inside an
    // inactive `ifdef does not define anything).
    if(!emitting()) {
        begin_skip_to_eol();
        return;
    }

    if(name == "define") {
        m_pending_name.clear();
        begin_define_name();
        return;
    }
    if(name == "undef") {
        begin_undef_name();
        return;
    }
    if(name == "include") {
        begin_include_arg();
        return;
    }
    if(name == "__FILE__") {
        // §22.13: SV-only. Expand to the path of the current input
        // file, as a quoted string literal.
        if(!m_driver.get_sv_mode()) {
            LOG_WARNING << "file \"" << m_filename << "\", line " << yylineno
                        << ": `__FILE__ is SystemVerilog-only (§22.13)";
        }
        emit(std::string("\"") + m_filename + "\"");
        return;
    }
    if(name == "__LINE__") {
        // §22.13: SV-only. Expand to the current input line number, as
        // a simple decimal number.
        if(!m_driver.get_sv_mode()) {
            LOG_WARNING << "file \"" << m_filename << "\", line " << yylineno
                        << ": `__LINE__ is SystemVerilog-only (§22.13)";
        }
        emit(std::to_string(yylineno));
        return;
    }
    if(name == "undefineall") {
        // §22.5.3, SV-only.
        if(!m_driver.get_sv_mode()) {
            LOG_WARNING << "file \"" << m_filename << "\", line " << yylineno
                        << ": `undefineall is SystemVerilog-only (§22.5.3)";
        }
        m_driver.macros().undef_all();
        return;
    }
    if(name == "resetall") {
        // §22.3: resets directive-state defaults but explicitly does
        // NOT touch the macro table. We have no directive state of our
        // own to reset (pass-through directives are owned by the main
        // scanner), so this is a no-op for us. We deliberately do not
        // emit `resetall verbatim — the main scanner does not accept
        // it (see verilog_scanner.ll), and forwarding it would just
        // surface as a parse error downstream.
        return;
    }

    // Pass-through directives — emit `<name> verbatim; the args that
    // follow on the same line fall through INITIAL rules and are
    // emitted as text.
    if(pass_through_directives().count(name)) {
        emit("`");
        emit(name);
        return;
    }

    // Macro use — expand if defined, warn-and-pass-through otherwise.
    Macro *m = m_driver.macros().find(name);
    if(m) {
        if(m->has_args) {
            // Function-like: must be followed by '(' (whitespace OK).
            // Capture the macro and switch to MACRO_CALL_PRE.
            m_call_macro = m;
            m_call_actuals.clear();
            m_call_depth = 0;
            begin_macro_call_pre();
            return;
        }
        // Object-like: route through expand_body so SV body escapes
        // (`", `\`", ``) are processed even without formals.
        if(m->expansion_depth >= kMaxExpansionDepth) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '" << name
                      << "' expansion depth exceeded " << kMaxExpansionDepth
                      << " — likely infinite recursion";
            return;
        }
        const std::string expanded = expand_body(*m, /*actuals*/ {}, m_driver.get_sv_mode());
        auto stream = std::make_unique<std::istringstream>(expanded);
        push_buffer(std::move(stream), m_filename, m);
        return;
    }

    LOG_WARNING << "file \"" << m_filename << "\", line " << yylineno
                << ": unknown compiler directive '`" << name << "' — passing through";
    emit("`");
    emit(name);
}

void PreprocessorScanner::define_finish()
{
    if(m_pending_name.empty()) {
        return; // already reported
    }
    m_driver.macros().define(m_pending_name, m_pending_has_args, m_pending_formals, m_pending_body,
                             m_filename, yylineno);
    m_pending_name.clear();
    m_pending_body.clear();
    m_pending_formals.clear();
    m_pending_has_args = false;
}

// ---------------------------------------------------------------------
// Function-like macro call argument collection.
//
// On entry to MACRO_CALL_PRE we have m_call_macro pointing at the macro
// being invoked and m_call_actuals empty. The Flex rules consume the
// arg list character by character, invoking the helpers below. The
// invariant we maintain is m_call_depth = number of unmatched ( / [ /
// { that have been opened since the outer call's '(' — so a ',' or
// ')' is a separator/terminator iff m_call_depth == 0 (§22.5.1: actual
// args may not contain comma or right-paren outside matched pairs).
// ---------------------------------------------------------------------

void PreprocessorScanner::macro_call_start(Macro *m)
{
    m_call_macro = m;
    m_call_actuals.clear();
    m_call_actuals.emplace_back();
    m_call_depth = 0;
}

void PreprocessorScanner::macro_call_arg_char(char c)
{
    if(m_call_actuals.empty()) {
        m_call_actuals.emplace_back();
    }
    m_call_actuals.back().push_back(c);
}

void PreprocessorScanner::macro_call_arg_text(const char *t)
{
    if(m_call_actuals.empty()) {
        m_call_actuals.emplace_back();
    }
    m_call_actuals.back().append(t);
}

void PreprocessorScanner::macro_call_arg_open(char c)
{
    macro_call_arg_char(c);
    ++m_call_depth;
}

namespace
{

std::string trim_ws(const std::string &s)
{
    const auto is_ws = [](unsigned char c) { return std::isspace(c); };
    auto start = s.begin();
    while(start != s.end() && is_ws(static_cast<unsigned char>(*start))) {
        ++start;
    }
    auto end = s.end();
    while(end != start && is_ws(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    return std::string(start, end);
}

} // namespace

bool PreprocessorScanner::macro_call_arg_close(char c)
{
    if(m_call_depth == 0) {
        if(c != ')') {
            // Mismatched ] or } at depth 0 — just include as text.
            macro_call_arg_char(c);
            return false;
        }
        // Outer ')' — finish the call.
        Macro *m = m_call_macro;
        m_call_macro = nullptr;

        if(!m) {
            return true; // defensive
        }

        // §22.5.1 says actuals are substituted "literally", but the
        // spec examples (`max(p+q, r+s) etc.) imply the whitespace
        // adjacent to commas/parens is part of the *separator*, not
        // the actual. Trim each captured actual accordingly.
        for(auto &a : m_call_actuals) {
            a = trim_ws(a);
        }

        // Empty actual list (`X()) maps to zero actuals when the macro
        // was declared with no formals.
        if(m_call_actuals.size() == 1 && m_call_actuals.front().empty() && m->formals.empty()) {
            m_call_actuals.clear();
        }

        // §22.5.1: too many actuals is always an error.
        if(m_call_actuals.size() > m->formals.size()) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                      << m->name << "' expects " << m->formals.size() << " argument(s), got "
                      << m_call_actuals.size();
            m_call_actuals.clear();
            return true;
        }

        // §22.5.1 default-argument rules:
        //   - if an actual is empty/whitespace-only and the formal has
        //     a default, substitute the default
        //   - if an actual is missing (fewer actuals than formals) and
        //     the formal has a default, substitute the default
        //   - missing actual with no default => error
        std::vector<std::string> resolved;
        resolved.reserve(m->formals.size());
        for(size_t i = 0; i < m->formals.size(); ++i) {
            const Formal &f = m->formals[i];
            if(i >= m_call_actuals.size()) {
                if(!f.has_default) {
                    LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                              << m->name << "' missing argument '" << f.name
                              << "' (no default specified)";
                    m_call_actuals.clear();
                    return true;
                }
                resolved.push_back(f.default_text);
                continue;
            }
            const std::string &actual = m_call_actuals[i];
            if(actual.empty() && f.has_default) {
                resolved.push_back(f.default_text);
            } else {
                resolved.push_back(actual);
            }
        }

        if(m->expansion_depth >= kMaxExpansionDepth) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                      << m->name << "' expansion depth exceeded " << kMaxExpansionDepth
                      << " — likely infinite recursion";
            m_call_actuals.clear();
            return true;
        }

        const std::string expanded = expand_body(*m, resolved, m_driver.get_sv_mode());
        m_call_actuals.clear();
        auto stream = std::make_unique<std::istringstream>(expanded);
        push_buffer(std::move(stream), m_filename, m);
        return true;
    }
    macro_call_arg_char(c);
    --m_call_depth;
    return false;
}

void PreprocessorScanner::macro_call_arg_comma()
{
    if(m_call_depth == 0) {
        m_call_actuals.emplace_back();
    } else {
        macro_call_arg_char(',');
    }
}

void PreprocessorScanner::macro_call_abort()
{
    m_call_macro = nullptr;
    m_call_actuals.clear();
    m_call_depth = 0;
}

void PreprocessorScanner::undef_apply(const char *name) { m_driver.macros().undef(name); }

void PreprocessorScanner::include_open(const char *quoted_or_angled)
{
    std::string raw(quoted_or_angled);
    if(raw.size() < 2) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": malformed `include argument";
        return;
    }
    const bool is_angle = (raw.front() == '<' && raw.back() == '>');
    const bool is_quote = (raw.front() == '"' && raw.back() == '"');
    if(!is_angle && !is_quote) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": `include argument must be \"file\" or <file>";
        return;
    }
    if(is_angle && !m_driver.get_sv_mode()) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": angle-bracket `include <...> requires SystemVerilog mode (§22.4)";
        return;
    }

    const std::string name = raw.substr(1, raw.size() - 2);
    const std::string resolved = m_driver.resolve_quoted_include(name);
    if(resolved.empty()) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": cannot find include file \"" << name << "\"";
        return;
    }

    if(static_cast<int>(m_frames.size()) >= m_driver.max_include_depth()) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": `include nesting depth exceeds limit (" << m_driver.max_include_depth()
                  << ")";
        return;
    }

    auto stream = std::make_unique<std::ifstream>(resolved);
    if(!stream->good()) {
        LOG_ERROR << "could not open include file \"" << resolved << "\"";
        return;
    }

    // §22.12: emit a level-1 line marker so downstream diagnostics point
    // into the included file.
    if(emitting()) {
        m_driver.emit_line_marker(*m_output, 1, resolved, 1);
    }
    push_buffer(std::move(stream), resolved, nullptr);
}

void PreprocessorScanner::include_expand_macro(const char *name_after_tick)
{
    // §22.5.1 last paragraph: `include can be followed by a macro that
    // expands to the quoted (or angle-bracket) filename. SV-only, and
    // we only support object-like macros here; a function-like macro
    // would require gathering arguments mid-INCLUDE_ARG state which is
    // out of scope.
    const std::string name(name_after_tick);
    if(!m_driver.get_sv_mode()) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": macro-as-filename in `include is SystemVerilog-only (§22.5.1)";
        return;
    }
    Macro *m = m_driver.macros().find(name);
    if(!m) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": undefined macro '"
                  << name << "' in `include argument";
        return;
    }
    if(m->has_args) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": function-like macro '"
                  << name << "' is not supported as an `include argument";
        return;
    }
    if(m->expansion_depth >= kMaxExpansionDepth) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '" << name
                  << "' expansion depth exceeded " << kMaxExpansionDepth;
        return;
    }
    const std::string expanded = expand_body(*m, /*actuals*/ {}, m_driver.get_sv_mode());
    auto stream = std::make_unique<std::istringstream>(expanded);
    push_buffer(std::move(stream), m_filename, m);
}

void PreprocessorScanner::cond_apply_name(const char *name)
{
    const bool defined = m_driver.macros().is_defined(name);
    switch(m_pending_cond_kind) {
    case CondKind::Ifdef:
        m_driver.conditionals().push_ifdef(defined, m_filename, yylineno);
        break;
    case CondKind::Ifndef:
        m_driver.conditionals().push_ifndef(defined, m_filename, yylineno);
        break;
    case CondKind::Elsif:
        m_driver.conditionals().handle_elsif(defined, m_filename, yylineno);
        break;
    }
}

void PreprocessorScanner::cond_apply_else()
{
    m_driver.conditionals().handle_else(m_filename, yylineno);
}

void PreprocessorScanner::cond_apply_endif()
{
    m_driver.conditionals().handle_endif(m_filename, yylineno);
}

void PreprocessorScanner::push_buffer(std::unique_ptr<std::istream> stream,
                                      const std::string &filename, Macro *macro_or_null)
{
    // 16384 == Flex's default YY_BUF_SIZE; the macro is only in scope
    // inside the generated scanner, so re-state it here.
    struct yy_buffer_state *buf = yy_create_buffer(stream.get(), 16384);
    yypush_buffer_state(buf);

    if(macro_or_null) {
        macro_or_null->expansion_depth++;
    }

    Frame f;
    f.buffer = buf;
    f.owned_stream = std::move(stream);
    f.filename = m_filename;
    f.saved_line = yylineno;
    f.expanding_macro = macro_or_null;
    m_frames.push_back(std::move(f));

    m_filename = filename;
    yylineno = 1;
}

bool PreprocessorScanner::on_eof()
{
    if(m_frames.empty()) {
        return false; // no pushed buffer — top-level EOF
    }

    Frame f = std::move(m_frames.back());
    m_frames.pop_back();

    if(f.expanding_macro) {
        f.expanding_macro->expansion_depth--;
    } else {
        // Returning from a `include — emit a level-2 marker (§22.12).
        if(emitting()) {
            m_driver.emit_line_marker(*m_output, f.saved_line, f.filename, 2);
        }
    }

    yypop_buffer_state();
    m_filename = f.filename;
    yylineno = f.saved_line;
    // We still have frames? Then there is another pushed buffer beneath.
    // Otherwise we are back to the top-level buffer (still scannable).
    return true;
}

} // namespace Parser
} // namespace Veriparse
