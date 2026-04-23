// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <veriparse/parser/verilog.hpp>
#include <parser/verilog/verilog_driver.hpp>


namespace Veriparse {
namespace Parser {

Verilog::Verilog() {
	m_driver = new VerilogDriver();
}

Verilog::~Verilog() {
	delete m_driver;
}

int Verilog::parse(const std::string &filename) {
	return m_driver->parse(filename);
}

int Verilog::parse(std::istream &iss) {
	return m_driver->parse(iss);
}

AST::Node::Ptr Verilog::get_source() {
	return m_driver->get_source_node();
}

}
}
