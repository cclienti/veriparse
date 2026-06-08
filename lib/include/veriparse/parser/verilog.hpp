// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PARSER_VERILOG
#define VERIPARSE_PARSER_VERILOG

#include <iostream>
#include <string>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Parser
{

class VerilogDriver;

class Verilog
{
public:
    Verilog();

    virtual ~Verilog();

    /**
     * parse - parse from a file
     * @param filename - valid string with input file
     */
    int parse(const std::string &filename);

    /**
     * Enable SystemVerilog mode (must be called before parse)
     */
    void set_sv_mode(bool sv_mode);
    bool get_sv_mode() const;

    /**
     * parse - parse from a c++ input stream
     * @param is - std::istream&, valid input stream
     */
    int parse(std::istream &iss);

    /**
     * return the parsed source top node
     */
    AST::Node::Ptr get_source(void);

private:
    VerilogDriver *m_driver = nullptr;
};

} // namespace Parser
} // namespace Veriparse

#endif
