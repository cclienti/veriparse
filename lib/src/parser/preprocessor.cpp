// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <veriparse/parser/preprocessor.hpp>
#include <parser/preprocessor/pp_driver.hpp>

namespace Veriparse
{
namespace Parser
{

Preprocessor::Preprocessor() : m_driver(std::make_unique<PreprocessorDriver>()) {}

Preprocessor::~Preprocessor() = default;

void Preprocessor::set_sv_mode(bool sv_mode) { m_driver->set_sv_mode(sv_mode); }
bool Preprocessor::get_sv_mode() const { return m_driver->get_sv_mode(); }

void Preprocessor::add_include_dir(const std::string &dir) { m_driver->add_include_dir(dir); }

void Preprocessor::define(const std::string &name, const std::string &body)
{
    m_driver->define(name, body);
}
void Preprocessor::undef(const std::string &name) { m_driver->undef(name); }

int Preprocessor::preprocess(const std::string &filename, std::ostream &out)
{
    return m_driver->preprocess(filename, out);
}

int Preprocessor::preprocess(const std::vector<std::string> &filenames, std::ostream &out)
{
    return m_driver->preprocess(filenames, out);
}

int Preprocessor::preprocess(std::istream &in, const std::string &filename, std::ostream &out)
{
    return m_driver->preprocess(in, filename, out);
}

} // namespace Parser
} // namespace Veriparse
