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

/// Result of resolve_actuals: status discriminates the three §22.5.1
/// outcomes. The Ok arm populates `resolved`; the error arms populate
/// the diagnostic fields the caller needs to log a contextual message.
struct ResolveActualsResult
{
    enum class Status
    {
        Ok,
        TooMany,
        MissingNoDefault,
    };
    Status status{Status::Ok};
    /// Number of raw actuals after trimming/collapsing — what the
    /// caller wants to report in a "too many" diagnostic.
    size_t actual_count{0};
    /// Name of the missing formal — populated when Status::MissingNoDefault.
    std::string missing_formal;
    /// Final resolved actuals, one per formal — populated when Status::Ok.
    std::vector<std::string> resolved;
};

/// Apply §22.5.1 actual-resolution rules in one place so the
/// Flex-driven call path (macro_call_arg_close) and the textual call
/// path (expand_body's synthetic-string recursion) cannot drift:
///   - trim each raw actual (whitespace between commas/parens is part
///     of the separator, not the actual);
///   - `X()` with no formals collapses to the zero-actual case;
///   - too many actuals is an error;
///   - whitespace-only actual against a defaulted formal uses the
///     default; missing actual against a defaulted formal uses the
///     default; missing actual against a non-defaulted formal is an
///     error.
/// Caller logs the diagnostic so the file/line context fits its scope
/// (use-site `m_filename`/`yylineno` vs. defining-site of an outer
/// macro currently being expanded).
ResolveActualsResult resolve_actuals(const Macro &m, std::vector<std::string> raw_actuals)
{
    ResolveActualsResult r;
    for(auto &a : raw_actuals) {
        a = trim_ws(a);
    }
    if(raw_actuals.size() == 1 && raw_actuals.front().empty() && m.formals.empty()) {
        raw_actuals.clear();
    }
    r.actual_count = raw_actuals.size();
    if(raw_actuals.size() > m.formals.size()) {
        r.status = ResolveActualsResult::Status::TooMany;
        return r;
    }
    r.resolved.reserve(m.formals.size());
    for(size_t i = 0; i < m.formals.size(); ++i) {
        const Formal &f = m.formals[i];
        if(i >= raw_actuals.size()) {
            if(!f.has_default) {
                r.status = ResolveActualsResult::Status::MissingNoDefault;
                r.missing_formal = f.name;
                return r;
            }
            r.resolved.push_back(f.default_text);
            continue;
        }
        const std::string &actual = raw_actuals[i];
        if(actual.empty() && f.has_default) {
            r.resolved.push_back(f.default_text);
        } else {
            r.resolved.push_back(actual);
        }
    }
    return r;
}

/// Source-agnostic scanner for a function-like macro call's actual-argument
/// list, per the §22.5.1 grammar. Pulls characters from `next`, which
/// returns the next input character or -1 at end-of-input, starting one
/// character past the opening '('. It splits actuals on a depth-0 comma and
/// stops after consuming the matching depth-0 ')'. Counting is matched-pair
/// aware across () [] {}; "..." regular strings (with \-escapes) are opaque,
/// as are \escaped-identifiers, so neither hides a separator nor leaks one.
///
/// Returns ArgScan::Ok once the closing ')' has been consumed (the caller's
/// cursor is then positioned one past it), or ArgScan::Unterminated if the
/// input ends first. This is the single source of truth for the arg-list
/// grammar: feed it a string cursor (collect_actuals_textual, below) or, in
/// the future, a yyinput()-backed cursor to retire the Flex MACRO_CALL_ARGS
/// states — both then share one implementation and cannot drift.
///
/// What this deliberately does NOT model (and would need to, if a caller
/// ever fed it text containing these): synthetic-string delimiters `" ... `"
/// and token-paste `` carry no special meaning here — they fall through as
/// ordinary backtick/quote characters. Neither appears inside a real macro
/// call's actuals, but a future spec extension could expose the gap.
enum class ArgScan
{
    Ok,
    Unterminated,
};

template <class Next> ArgScan scan_actuals(Next &&next, std::vector<std::string> &actuals)
{
    actuals.clear();
    actuals.emplace_back();
    int depth = 0;
    for(int c = next(); c != -1; c = next()) {
        if(c == '(' || c == '[' || c == '{') {
            actuals.back().push_back(static_cast<char>(c));
            ++depth;
            continue;
        }
        if(c == ')') {
            if(depth == 0) {
                return ArgScan::Ok; // ')' consumed; caller cursor is now past it
            }
            actuals.back().push_back(static_cast<char>(c));
            --depth;
            continue;
        }
        if(c == ']' || c == '}') {
            actuals.back().push_back(static_cast<char>(c));
            if(depth > 0) {
                --depth;
            }
            continue;
        }
        if(c == ',' && depth == 0) {
            actuals.emplace_back();
            continue;
        }
        if(c == '"') {
            actuals.back().push_back('"');
            for(int d = next(); d != -1; d = next()) {
                actuals.back().push_back(static_cast<char>(d));
                if(d == '\\') {
                    const int e = next();
                    if(e == -1) {
                        break;
                    }
                    actuals.back().push_back(static_cast<char>(e));
                } else if(d == '"') {
                    break;
                }
            }
            continue;
        }
        if(c == '\\') {
            // \escaped-identifier runs to the next whitespace (§22.5.1).
            // The terminating whitespace is captured into the actual, just
            // as the earlier index-based scan left it for the following
            // iteration to append — trimming drops it later, so this is
            // faithful.
            actuals.back().push_back('\\');
            for(int d = next(); d != -1; d = next()) {
                actuals.back().push_back(static_cast<char>(d));
                if(std::isspace(static_cast<unsigned char>(d))) {
                    break;
                }
            }
            continue;
        }
        actuals.back().push_back(static_cast<char>(c));
    }
    return ArgScan::Unterminated;
}

/// Thin string-cursor adapter over scan_actuals: collects the actuals of a
/// function-like call embedded in `body`, starting at `start` (one past the
/// opening '('). On success `*end_pos` is set one past the matching ')'.
/// Returns false if the body ends before that ')'.
bool collect_actuals_textual(const std::string &body, size_t start,
                             std::vector<std::string> &actuals, size_t *end_pos)
{
    size_t i = start;
    const auto next = [&]() -> int {
        return i < body.size() ? static_cast<unsigned char>(body[i++]) : -1;
    };
    if(scan_actuals(next, actuals) != ArgScan::Ok) {
        return false;
    }
    *end_pos = i; // next() has consumed through the closing ')'
    return true;
}

/// Textual processing of a macro body before it gets pushed back into
/// the input stream:
///   - identifier-aware substitution of formal arguments (§22.5.1)
///   - SV-only body escapes:
///       `"        emits a plain "  (synthetic string delimiter, §22.5.1)
///       `\`"      emits \"          (escaped quote inside a synthetic string)
///       ``        emits nothing     (token paste, no whitespace introduced)
///   - SV-only nested-macro re-expansion inside `" ... `" synthetic
///     strings (§22.5.1): an inner `IDENT is looked up in the macro
///     table and its body inlined. Outside synthetic strings, nested
///     `IDENT references are left in the emitted body so the Flex
///     scanner picks them up on re-scan — but inside a synthetic
///     string the result reaches the main scanner as a regular "..."
///     literal which is opaque to directive scanning, so we must
///     resolve here.
///
/// Regular string literals "..." inside the body are copied verbatim
/// per §22.5.1 ("Macro substitution and argument substitution shall not
/// occur within string literals.") so a formal whose name happens to
/// appear inside a "..." is not substituted.
///
/// Function-like nested refs inside a synthetic string are also
/// expanded here, using collect_actuals_textual to mirror the Flex
/// MACRO_CALL_ARGS rules on the body string.
std::string expand_body(const Macro &macro, const std::vector<std::string> &actuals, bool sv_mode,
                        MacroTable *table = nullptr, int recursion_depth = 0,
                        bool textual_expand_refs = false)
{
    const std::string &body = macro.body;
    const std::vector<Formal> &formals = macro.formals;
    const bool has_formals = macro.has_args && !formals.empty();
    bool in_synthetic = false;
    // When expanding a nested macro inside a synthetic string the result
    // gets inlined as text instead of going back through Flex re-scan,
    // so we have to resolve every `IDENT here — not just the ones that
    // happen to sit inside a `"..."` region of the nested body.
    const auto must_expand_ref = [&]() { return in_synthetic || textual_expand_refs; };

    const auto isid_start = [](unsigned char ch) { return std::isalpha(ch) || ch == '_'; };
    const auto isid_cont = [](unsigned char ch) {
        return std::isalnum(ch) || ch == '_' || ch == '$';
    };

    std::string out;
    out.reserve(body.size() * 2);

    size_t i = 0;
    while(i < body.size()) {
        const char c = body[i];

        // SV-only body escapes (§22.5.1). Checked before regular-string
        // handling so a leading `" opens a synthetic-string region
        // rather than being treated as a stray backtick + " sequence.
        if(sv_mode && c == '`' && i + 1 < body.size()) {
            // `\`" — 4 chars: ` \ ` " → emit \" (does not toggle the
            // synthetic-string region — that's exactly what makes it
            // an *escape*).
            if(body[i + 1] == '\\' && i + 3 < body.size() && body[i + 2] == '`' &&
               body[i + 3] == '"') {
                out += "\\\"";
                i += 4;
                continue;
            }
            // `" — emit a plain quote and toggle the synthetic-string
            // region. The open and close use the same delimiter
            // (§22.5.1), so a simple toggle is faithful.
            if(body[i + 1] == '"') {
                out += '"';
                i += 2;
                in_synthetic = !in_synthetic;
                continue;
            }
            // `` — token paste, no character emitted.
            if(body[i + 1] == '`') {
                i += 2;
                continue;
            }
            // Inside a synthetic-string region (or once we have already
            // descended into a textual recursion), `IDENT is a nested
            // macro reference. Look it up and inline the body
            // (recursive, depth-capped). Object-like is substituted
            // directly; function-like looks ahead for '(' and runs the
            // textual arg collector below.
            if(must_expand_ref() && table != nullptr &&
               isid_start(static_cast<unsigned char>(body[i + 1]))) {
                size_t j = i + 1;
                while(j < body.size() && isid_cont(static_cast<unsigned char>(body[j]))) {
                    ++j;
                }
                const std::string ident = body.substr(i + 1, j - (i + 1));
                Macro *nested = table->find(ident);
                if(nested == nullptr) {
                    // Unknown reference — leave it literal; the spec
                    // allows the parser to surface the issue.
                    out += '`';
                    out += ident;
                    i = j;
                    continue;
                }
                if(nested->has_args) {
                    // Look ahead past whitespace for the opening '('.
                    // §22.5.1 permits whitespace between the macro name
                    // and the parenthesis. If '(' is absent the use is
                    // degenerate; emit literally and move on.
                    size_t k = j;
                    while(k < body.size() && std::isspace(static_cast<unsigned char>(body[k]))) {
                        ++k;
                    }
                    if(k >= body.size() || body[k] != '(') {
                        LOG_WARNING << "file \"" << macro.defined_filename << "\", line "
                                    << macro.defined_line << ": in body of macro '" << macro.name
                                    << "', function-like macro '" << ident
                                    << "' inside a synthetic string is missing its '('";
                        out += '`';
                        out += ident;
                        i = j;
                        continue;
                    }

                    // Gather raw actuals from after the '('. The
                    // collector mirrors the Flex MACRO_CALL_ARGS rules
                    // textually so commas inside matched pairs or
                    // string literals don't split actuals.
                    std::vector<std::string> raw_actuals;
                    size_t end_pos = 0;
                    if(!collect_actuals_textual(body, k + 1, raw_actuals, &end_pos)) {
                        LOG_ERROR << "file \"" << macro.defined_filename << "\", line "
                                  << macro.defined_line << ": in body of macro '" << macro.name
                                  << "', unterminated call to '" << ident
                                  << "' inside a synthetic string";
                        out += '`';
                        out += ident;
                        i = j;
                        continue;
                    }

                    // §22.5.1 actual-resolution shared with the
                    // Flex-driven call path. Diagnostics use the
                    // defining-site of the outer macro since that's the
                    // location of the actual textual error.
                    auto rr = resolve_actuals(*nested, std::move(raw_actuals));
                    if(rr.status == ResolveActualsResult::Status::TooMany) {
                        LOG_ERROR << "file \"" << macro.defined_filename << "\", line "
                                  << macro.defined_line << ": in body of macro '" << macro.name
                                  << "', macro '" << ident << "' expects " << nested->formals.size()
                                  << " argument(s), got " << rr.actual_count;
                        out += '`';
                        out += ident;
                        i = end_pos;
                        continue;
                    }
                    if(rr.status == ResolveActualsResult::Status::MissingNoDefault) {
                        LOG_ERROR << "file \"" << macro.defined_filename << "\", line "
                                  << macro.defined_line << ": in body of macro '" << macro.name
                                  << "', macro '" << ident << "' missing argument '"
                                  << rr.missing_formal << "' (no default specified)";
                        out += '`';
                        out += ident;
                        i = end_pos;
                        continue;
                    }
                    if(recursion_depth + 1 >= kMaxExpansionDepth) {
                        LOG_ERROR << "file \"" << macro.defined_filename << "\", line "
                                  << macro.defined_line << ": in body of macro '" << macro.name
                                  << "', macro '" << ident << "' synthetic-string expansion "
                                  << "depth exceeded " << kMaxExpansionDepth
                                  << " — likely infinite recursion";
                        out += '`';
                        out += ident;
                        i = end_pos;
                        continue;
                    }
                    out += expand_body(*nested, rr.resolved, sv_mode, table, recursion_depth + 1,
                                       /*textual_expand_refs*/ true);
                    i = end_pos;
                    continue;
                }
                if(recursion_depth + 1 >= kMaxExpansionDepth) {
                    LOG_ERROR << "file \"" << macro.defined_filename << "\", line "
                              << macro.defined_line << ": in body of macro '" << macro.name
                              << "', macro '" << ident << "' synthetic-string expansion depth "
                              << "exceeded " << kMaxExpansionDepth
                              << " — likely infinite recursion";
                    out += '`';
                    out += ident;
                    i = j;
                    continue;
                }
                out += expand_body(*nested, /*actuals*/ {}, sv_mode, table, recursion_depth + 1,
                                   /*textual_expand_refs*/ true);
                i = j;
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
        const std::string expanded =
            expand_body(*m, /*actuals*/ {}, m_driver.get_sv_mode(), &m_driver.macros());
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
            return macro_call_abort(); // defensive
        }

        // §22.5.1 actual-resolution shared with expand_body's
        // synthetic-string textual recursion. Diagnostics use the
        // use-site (current file/line) since that's where the bad
        // call was written.
        auto rr = resolve_actuals(*m, std::move(m_call_actuals));
        m_call_actuals.clear(); // restore the moved-from vector
        if(rr.status == ResolveActualsResult::Status::TooMany) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                      << m->name << "' expects " << m->formals.size() << " argument(s), got "
                      << rr.actual_count;
            return macro_call_abort();
        }
        if(rr.status == ResolveActualsResult::Status::MissingNoDefault) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                      << m->name << "' missing argument '" << rr.missing_formal
                      << "' (no default specified)";
            return macro_call_abort();
        }

        if(m->expansion_depth >= kMaxExpansionDepth) {
            LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '"
                      << m->name << "' expansion depth exceeded " << kMaxExpansionDepth
                      << " — likely infinite recursion";
            return macro_call_abort();
        }

        const std::string expanded =
            expand_body(*m, rr.resolved, m_driver.get_sv_mode(), &m_driver.macros());
        auto stream = std::make_unique<std::istringstream>(expanded);
        push_buffer(std::move(stream), m_filename, m);
        // §22.5.1 last paragraph: when the call came from inside an
        // `include argument, the expansion (which is supposed to be a
        // "..." or <...> string) must be re-fed to INCLUDE_ARG rather
        // than INITIAL. Returning false here keeps the Flex rule from
        // forcing BEGIN(INITIAL); we set the state ourselves.
        if(m_call_from_include) {
            m_call_from_include = false;
            begin_include_arg();
            return false;
        }
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

bool PreprocessorScanner::macro_call_abort()
{
    const bool from_include = m_call_from_include;
    m_call_macro = nullptr;
    m_call_actuals.clear();
    m_call_depth = 0;
    m_call_from_include = false;
    if(from_include) {
        // Errors during an `include `MACRO(...) sequence: drop the
        // rest of the directive line so trailing whitespace, comments,
        // or stray tokens don't leak into the emitted stream — matches
        // INCLUDE_TAIL's behaviour for malformed directive arguments.
        begin_skip_to_eol();
        return false;
    }
    return true;
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
    // expands to the quoted (or angle-bracket) filename. SV-only.
    // Object-like is resolved inline; function-like routes through the
    // MACRO_CALL_PRE/MACRO_CALL_ARGS state machine and comes back via
    // macro_call_arg_close, which puts us back in INCLUDE_ARG with the
    // expansion pushed as a buffer.
    // Recovery for any early-return error path: stay out of INCLUDE_ARG
    // so the next token on the directive line isn't re-classified as
    // another `include argument and reported a second time.
    const auto bail = [this]() { begin_skip_to_eol(); };
    const std::string name(name_after_tick);
    if(!m_driver.get_sv_mode()) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno
                  << ": macro-as-filename in `include is SystemVerilog-only (§22.5.1)";
        bail();
        return;
    }
    Macro *m = m_driver.macros().find(name);
    if(!m) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": undefined macro '"
                  << name << "' in `include argument";
        bail();
        return;
    }
    if(m->expansion_depth >= kMaxExpansionDepth) {
        LOG_ERROR << "file \"" << m_filename << "\", line " << yylineno << ": macro '" << name
                  << "' expansion depth exceeded " << kMaxExpansionDepth;
        bail();
        return;
    }
    if(m->has_args) {
        // Function-like: arm the call-collection state and switch into
        // MACRO_CALL_PRE. We come back to INCLUDE_ARG once the matching
        // ')' fires macro_call_arg_close.
        m_call_macro = m;
        m_call_actuals.clear();
        m_call_depth = 0;
        m_call_from_include = true;
        begin_macro_call_pre();
        return;
    }
    const std::string expanded =
        expand_body(*m, /*actuals*/ {}, m_driver.get_sv_mode(), &m_driver.macros());
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
