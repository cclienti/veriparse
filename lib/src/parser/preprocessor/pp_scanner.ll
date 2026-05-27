%{
// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22 and IEEE Std 1364-2005 §19.

#include <parser/preprocessor/pp_scanner.hpp>
#include <parser/preprocessor/pp_driver.hpp>
#include <veriparse/logger/logger.hpp>

#define YY_NO_UNISTD_H

%}

%option yyclass="Veriparse::Parser::PreprocessorScanner"
%option noyywrap
%option yylineno
%option c++

%x STRING
%x BLOCK_COMMENT
%x LINE_COMMENT
%x DEFINE_NAME
%x DEFINE_FORMALS
%x DEFINE_BODY_LEAD
%x DEFINE_BODY
%x DEFINE_BODY_STRING
%x UNDEF_NAME
%x INCLUDE_ARG
%x INCLUDE_TAIL
%x COND_NAME
%x SKIP_TO_EOL
%x MACRO_CALL_PRE
%x MACRO_CALL_ARGS
%x MACRO_CALL_STRING

ID  [a-zA-Z_][a-zA-Z0-9_$]*

%%

	/* String literals at the top level (§5.9 / §22.5.1: no macro
	 * substitution inside ordinary strings). */
"\""              {emit("\""); BEGIN(STRING);}
<STRING>\\\"      {emit(yytext);}
<STRING>\\\\      {emit(yytext);}
<STRING>"\""      {emit("\""); BEGIN(INITIAL);}
<STRING>\n        {emit("\n");}
<STRING>.         {emit(yytext);}

	/* Block comment — pass through verbatim. */
"/*"              {emit("/*"); BEGIN(BLOCK_COMMENT);}
<BLOCK_COMMENT>"*/" {emit("*/"); BEGIN(INITIAL);}
<BLOCK_COMMENT>\n {emit("\n");}
<BLOCK_COMMENT>.  {emit(yytext);}

	/* Line comment — pass through verbatim. */
"//"              {emit("//"); BEGIN(LINE_COMMENT);}
<LINE_COMMENT>\n  {emit("\n"); BEGIN(INITIAL);}
<LINE_COMMENT>.   {emit(yytext);}

	/* `<ident> — directive keyword, defined macro, or unknown. */
"`"{ID}           {handle_backtick(yytext + 1);}

	/* `define NAME (§22.5.1). The lookahead /"(" picks the function-like
	 * form because §22.5.1 requires: "The left parenthesis shall follow
	 * the text macro name immediately, with no space in between." A
	 * whitespace between the name and '(' means object-like. */
<DEFINE_NAME>[ \t]+   {/* skip whitespace between `define and name */}
<DEFINE_NAME>{ID}/"(" {define_set_name_func(yytext); BEGIN(DEFINE_FORMALS);}
<DEFINE_NAME>{ID}     {define_set_name(yytext); BEGIN(DEFINE_BODY_LEAD);}
<DEFINE_NAME>\n       {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                 << ": `define with no macro name";
                       BEGIN(INITIAL);}
<DEFINE_NAME>.        {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                 << ": invalid character '" << yytext << "' after `define";
                       BEGIN(SKIP_TO_EOL);}

	/* Formal arg list of a function-like macro. §22.5.1: simple
	 * identifiers separated by commas, enclosed in parentheses. */
<DEFINE_FORMALS>"("   {/* eat the opener */}
<DEFINE_FORMALS>[ \t]+ {/* skip */}
<DEFINE_FORMALS>{ID}  {define_add_formal(yytext);}
<DEFINE_FORMALS>","   {/* next formal */}
<DEFINE_FORMALS>")"   {BEGIN(DEFINE_BODY_LEAD);}
<DEFINE_FORMALS>\n    {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                 << ": newline in `define formal-argument list";
                       BEGIN(INITIAL);}
<DEFINE_FORMALS>.     {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                 << ": invalid character '" << yytext
                                 << "' in `define formal-argument list";
                       BEGIN(SKIP_TO_EOL);}

	/* Strip whitespace and stripped // comments between the macro name
	 * and the actual body. The spec example "`define wordsize 8" sets
	 * the body to "8" (not " 8"), so the single conventional separator
	 * between name and body is not part of the body. */
<DEFINE_BODY_LEAD>[ \t]+      {/* skip leading whitespace before body */}
<DEFINE_BODY_LEAD>"//"[^\n]*  {/* §22.5.1: line comment stripped */}
<DEFINE_BODY_LEAD>\n          {define_finish(); BEGIN(INITIAL);}
<DEFINE_BODY_LEAD>.           {yyless(0); BEGIN(DEFINE_BODY);}

	/* `define body. §22.5.1:
	 *   - "\\<newline>" stored as <newline>, body collection continues
	 *   - // comment stripped from stored body
	 *   - first unescaped newline terminates the body
	 *   - strings inside the body are kept verbatim (sub-state) */
<DEFINE_BODY>\\\n        {define_append_body_char('\n');}
<DEFINE_BODY>"//"[^\n]*  {/* §22.5.1: line comment stripped */}
<DEFINE_BODY>"\""        {define_append_body_char('"'); BEGIN(DEFINE_BODY_STRING);}
<DEFINE_BODY>\n          {define_finish(); BEGIN(INITIAL);}
<DEFINE_BODY>.           {define_append_body(yytext);}

	/* String literal inside a macro body — preserve all characters as
	 * body text so the //-stripping rule above never fires for // that
	 * happens to appear inside a literal string. */
<DEFINE_BODY_STRING>\\\"  {define_append_body(yytext);}
<DEFINE_BODY_STRING>\\\\  {define_append_body(yytext);}
<DEFINE_BODY_STRING>"\""  {define_append_body_char('"'); BEGIN(DEFINE_BODY);}
<DEFINE_BODY_STRING>\n    {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                     << ": unterminated string in macro body";
                           define_finish(); BEGIN(INITIAL);}
<DEFINE_BODY_STRING>.     {define_append_body(yytext);}

	/* `undef NAME (§22.5.2) */
<UNDEF_NAME>[ \t]+  {/* skip */}
<UNDEF_NAME>{ID}    {undef_apply(yytext); BEGIN(SKIP_TO_EOL);}
<UNDEF_NAME>\n      {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                               << ": `undef with no macro name";
                     BEGIN(INITIAL);}
<UNDEF_NAME>.       {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                               << ": invalid character '" << yytext << "' after `undef";
                     BEGIN(SKIP_TO_EOL);}

	/* `include arg (§22.4). Quoted form always allowed; angle-bracket
	 * form (SV-only) is gated by include_open. We capture the literal
	 * argument here, then in INCLUDE_TAIL consume the rest of the
	 * include line on the ORIGINAL buffer before pushing the new
	 * buffer — otherwise the start condition would apply to the
	 * pushed buffer and swallow its first line. */
<INCLUDE_ARG>[ \t]+            {/* skip */}
<INCLUDE_ARG>"\""[^"\n]*"\""   {m_pending_name = yytext; BEGIN(INCLUDE_TAIL);}
<INCLUDE_ARG>"<"[^>\n]*">"     {m_pending_name = yytext; BEGIN(INCLUDE_TAIL);}
<INCLUDE_ARG>\n                {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                          << ": missing `include argument";
                                BEGIN(INITIAL);}
<INCLUDE_ARG>.                 {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                          << ": invalid character '" << yytext
                                          << "' after `include";
                                BEGIN(SKIP_TO_EOL);}

<INCLUDE_TAIL>[ \t]+           {/* §22.4: only whitespace/comment allowed */}
<INCLUDE_TAIL>"//"[^\n]*       {/* trailing line comment is allowed */}
<INCLUDE_TAIL>\n               {const std::string arg = m_pending_name;
                                m_pending_name.clear();
                                BEGIN(INITIAL);
                                include_open(arg.c_str());}
<INCLUDE_TAIL>.                {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                          << ": unexpected text after `include argument";
                                m_pending_name.clear();
                                BEGIN(SKIP_TO_EOL);}

	/* `ifdef NAME / `ifndef NAME / `elsif NAME (§22.6) */
<COND_NAME>[ \t]+  {/* skip */}
<COND_NAME>{ID}    {cond_apply_name(yytext); BEGIN(INITIAL);}
<COND_NAME>\n      {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                              << ": missing macro name after `ifdef/`ifndef/`elsif";
                    BEGIN(INITIAL);}
<COND_NAME>.       {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                              << ": invalid character '" << yytext
                              << "' after `ifdef/`ifndef/`elsif";
                    BEGIN(SKIP_TO_EOL);}

	/* Skip the rest of a directive line (used in inactive `ifdef
	 * branches, after error recovery, and after directives whose
	 * trailing args we want to discard). */
<SKIP_TO_EOL>\\\n  {/* line continuation, keep skipping */}
<SKIP_TO_EOL>\n    {BEGIN(INITIAL);}
<SKIP_TO_EOL>.     {/* discard */}

	/* Function-like macro call: between the backtick name and '('
	 * §22.5.1 allows whitespace. Newlines are not addressed by the
	 * spec; we accept them (lenient) so multi-line invocations work. */
<MACRO_CALL_PRE>[ \t]+ {/* skip */}
<MACRO_CALL_PRE>\n     {/* allow newline before '(' */}
<MACRO_CALL_PRE>"("    {BEGIN(MACRO_CALL_ARGS); m_call_actuals.clear(); m_call_actuals.emplace_back(); m_call_depth = 0;}
<MACRO_CALL_PRE>.      {LOG_ERROR << "file \"" << get_filename() << "\", line " << yylineno
                                  << ": function-like macro requires '(' after its name";
                        macro_call_abort();
                        BEGIN(INITIAL);}

	/* Inside a macro call arg list. Matched-pair-aware counting per
	 * §22.5.1: comma at depth-0 separates actuals, ')' at depth-0
	 * terminates the call. (), [], {}, "", and \<id> all hide commas
	 * and closing-parens from the separator logic. */
<MACRO_CALL_ARGS>"("   {macro_call_arg_open('(');}
<MACRO_CALL_ARGS>")"   {if (macro_call_arg_close(')')) BEGIN(INITIAL);}
<MACRO_CALL_ARGS>","   {macro_call_arg_comma();}
<MACRO_CALL_ARGS>"["   {macro_call_arg_open('[');}
<MACRO_CALL_ARGS>"]"   {(void)macro_call_arg_close(']');}
<MACRO_CALL_ARGS>"{"   {macro_call_arg_open('{');}
<MACRO_CALL_ARGS>"}"   {(void)macro_call_arg_close('}');}
<MACRO_CALL_ARGS>"\""  {macro_call_arg_char('"'); BEGIN(MACRO_CALL_STRING);}
<MACRO_CALL_ARGS>\\[^ \t\r\n]+  {macro_call_arg_text(yytext);}
<MACRO_CALL_ARGS>\n    {macro_call_arg_char('\n');}
<MACRO_CALL_ARGS>.     {macro_call_arg_char(yytext[0]);}

	/* String literal inside a macro-call argument list — commas and
	 * parens inside are part of the actual, not separators. */
<MACRO_CALL_STRING>\\\"  {macro_call_arg_text(yytext);}
<MACRO_CALL_STRING>\\\\  {macro_call_arg_text(yytext);}
<MACRO_CALL_STRING>"\""  {macro_call_arg_char('"'); BEGIN(MACRO_CALL_ARGS);}
<MACRO_CALL_STRING>\n    {macro_call_arg_char('\n');}
<MACRO_CALL_STRING>.     {macro_call_arg_char(yytext[0]);}

	/* End of input on any buffer — pop if there is a pushed buffer,
	 * terminate otherwise. */
<*><<EOF>>         {if (!on_eof()) yyterminate();}

	/* Default pass-through. */
\n                 {emit("\n");}
.                  {emit(yytext);}

%%

// State-entry helpers exposed in pp_scanner.hpp. Defined here so the
// %x start-condition constants are visible.

namespace Veriparse {
namespace Parser {

void PreprocessorScanner::begin_initial()         {BEGIN(INITIAL);}
void PreprocessorScanner::begin_define_name()     {BEGIN(DEFINE_NAME);}
void PreprocessorScanner::begin_undef_name()      {BEGIN(UNDEF_NAME);}
void PreprocessorScanner::begin_include_arg()     {BEGIN(INCLUDE_ARG);}
void PreprocessorScanner::begin_cond_name()       {BEGIN(COND_NAME);}
void PreprocessorScanner::begin_skip_to_eol()     {BEGIN(SKIP_TO_EOL);}
void PreprocessorScanner::begin_macro_call_pre()  {BEGIN(MACRO_CALL_PRE);}

}
}
