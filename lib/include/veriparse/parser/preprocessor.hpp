// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PARSER_PREPROCESSOR
#define VERIPARSE_PARSER_PREPROCESSOR

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace Veriparse
{
namespace Parser
{

class PreprocessorDriver;

class Preprocessor
{
public:
    Preprocessor();
    ~Preprocessor();

    /**
     * Enable SystemVerilog mode. Gates SV-only directives and semantics
     * (IEEE Std 1800-2017 §22): `__FILE__, `__LINE__, `undefineall,
     * default macro arg values, `" / ` ` / `\`", angle-bracket `include.
     */
    void set_sv_mode(bool sv_mode);
    bool get_sv_mode() const;

    /**
     * Append a directory to the `include search path. Order of
     * registration is the order of search.
     */
    void add_include_dir(const std::string &dir);

    /**
     * CLI-style macro predefines, equivalent to `define on the command
     * line. body may be empty.
     */
    void define(const std::string &name, const std::string &body = "");
    void undef(const std::string &name);

    /**
     * Preprocess one file. Writes the preprocessed text to out. Returns
     * 0 on success, nonzero on error (errors go to LOG_ERROR).
     */
    int preprocess(const std::string &filename, std::ostream &out);

    /**
     * Preprocess multiple files, concatenated in order with `line
     * markers between them. Equivalent to feeding `iverilog -E` a list.
     */
    int preprocess(const std::vector<std::string> &filenames, std::ostream &out);

    /**
     * Preprocess from an arbitrary stream. filename is used for
     * `__FILE__, `line markers, and diagnostics.
     */
    int preprocess(std::istream &in, const std::string &filename, std::ostream &out);

private:
    std::unique_ptr<PreprocessorDriver> m_driver;
};

} // namespace Parser
} // namespace Veriparse

#endif
