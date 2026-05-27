// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22 and IEEE Std 1364-2005 §19.
#ifndef VERIPARSE_PREPROCESSOR_SCANNER_HPP
#define VERIPARSE_PREPROCESSOR_SCANNER_HPP

// The yyFlexLexer macro trick (same pattern as verilog_scanner.hpp) lets
// us host multiple Flex-generated scanners in the same library. Paired
// with -P Preprocessor in the build to namespace the runtime symbols.
#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer PreprocessorFlexLexer
#include <FlexLexer.h>
#endif

#include <parser/preprocessor/pp_macro.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Forward-declare the Flex per-buffer state. FlexLexer.h's definition
// of YY_BUFFER_STATE / yy_buffer_state can vary between Flex versions,
// so we rely on the struct name only.
struct yy_buffer_state;

namespace Veriparse
{
namespace Parser
{

class PreprocessorDriver;

class PreprocessorScanner : public yyFlexLexer
{
public:
    PreprocessorScanner(std::istream *in, std::ostream *out, const std::string &filename,
                        PreprocessorDriver &driver);

    ~PreprocessorScanner() override;

    using yyFlexLexer::yylex;
    int yylex() override;

    const std::string &get_filename() const { return m_filename; }
    int get_line() const { return yylineno; }

    /// True iff the conditional stack lets us emit.
    bool emitting() const;

    // --- helpers called from Flex rule actions ---

    /// Emit text to the output, gated on emitting().
    void emit(const char *text);
    void emit(const std::string &text);

    /// Dispatch on a `<ident> seen in INITIAL state. May change start
    /// condition (for multi-line directives) or push a new buffer
    /// (for macro expansion).
    void handle_backtick(const char *name_after_tick);

    // Body collection (DEFINE_NAME/DEFINE_BODY rules).
    void define_set_name(const char *name)
    {
        m_pending_name = name;
        m_pending_has_args = false;
        m_pending_formals.clear();
    }
    void define_set_name_func(const char *name)
    {
        m_pending_name = name;
        m_pending_has_args = true;
        m_pending_formals.clear();
    }
    void define_add_formal(const char *name)
    {
        Formal f;
        f.name = name;
        m_pending_formals.push_back(std::move(f));
    }
    void define_mark_formal_default()
    {
        if(!m_pending_formals.empty()) {
            m_pending_formals.back().has_default = true;
        }
        m_pending_default_depth = 0;
    }
    void define_append_default(const char *text)
    {
        if(!m_pending_formals.empty()) {
            m_pending_formals.back().default_text.append(text);
        }
    }
    void define_append_default_char(char c)
    {
        if(!m_pending_formals.empty()) {
            m_pending_formals.back().default_text.push_back(c);
        }
    }
    int default_depth() const { return m_pending_default_depth; }
    void default_inc_depth() { ++m_pending_default_depth; }
    void default_dec_depth() { --m_pending_default_depth; }

    void define_append_body(const char *text) { m_pending_body += text; }
    void define_append_body_char(char c) { m_pending_body += c; }
    void define_finish();

    // Function-like macro call argument collection.
    void macro_call_start(Macro *m);
    void macro_call_arg_char(char c);
    void macro_call_arg_text(const char *t);
    void macro_call_arg_open(char c);
    bool macro_call_arg_close(char c); ///< returns true on outer ')'
    void macro_call_arg_comma();
    void macro_call_abort(); ///< called on error mid-call

    // `undef NAME (UNDEF_NAME rule).
    void undef_apply(const char *name);

    // `include arg (INCLUDE_ARG rule).
    void include_open(const char *quoted_or_angled);
    /// Object-like macro use inside an `include argument (§22.5.1).
    /// Pushes the macro expansion onto the buffer stack so the
    /// subsequent INCLUDE_ARG rules match the expanded "..." literal.
    void include_expand_macro(const char *name_after_tick);

    // `ifdef/`ifndef/`elsif NAME (COND_NAME rule).
    void cond_set_kind_ifdef() { m_pending_cond_kind = CondKind::Ifdef; }
    void cond_set_kind_ifndef() { m_pending_cond_kind = CondKind::Ifndef; }
    void cond_set_kind_elsif() { m_pending_cond_kind = CondKind::Elsif; }
    void cond_apply_name(const char *name);

    // `else / `endif (no arg).
    void cond_apply_else();
    void cond_apply_endif();

    /// `<<EOF>>` handler — pop a pushed buffer if any. Returns true if
    /// scanning should continue (there is still a buffer to read from).
    bool on_eof();

    // State-entry helpers. Defined in pp_scanner.ll's user-code section
    // where the Flex %x state constants are visible. Called from
    // directive handlers in pp_scanner_impl.cpp.
    void begin_initial();
    void begin_define_name();
    void begin_define_formals();
    void begin_define_formal_default();
    void begin_define_formal_default_string();
    void begin_undef_name();
    void begin_include_arg();
    void begin_cond_name();
    void begin_skip_to_eol();
    void begin_macro_call_pre();

private:
    enum class CondKind
    {
        Ifdef,
        Ifndef,
        Elsif
    };

    struct Frame
    {
        struct yy_buffer_state *buffer;
        std::unique_ptr<std::istream> owned_stream;
        std::string filename;
        int saved_line;
        Macro *expanding_macro;
    };

    void push_buffer(std::unique_ptr<std::istream> stream, const std::string &filename,
                     Macro *macro_or_null);

    PreprocessorDriver &m_driver;
    std::ostream *m_output;
    std::string m_filename;
    std::vector<Frame> m_frames;

    // Pending state for multi-token directives.
    std::string m_pending_name;
    std::string m_pending_body;
    std::vector<Formal> m_pending_formals;
    bool m_pending_has_args{false};
    int m_pending_default_depth{0}; ///< bracket depth inside a default value
    CondKind m_pending_cond_kind{CondKind::Ifdef};

    // Pending state for a function-like macro call being parsed.
    Macro *m_call_macro{nullptr};
    std::vector<std::string> m_call_actuals;
    int m_call_depth{0}; ///< nesting depth inside the outer (
};

} // namespace Parser
} // namespace Veriparse

#endif
