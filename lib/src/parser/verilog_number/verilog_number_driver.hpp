#ifndef VERIPARSE_VERILOG_NUMBER_DRIVER_HPP
#define VERIPARSE_VERILOG_NUMBER_DRIVER_HPP

#include <string>
#include <cstddef>
#include <istream>
#include <string>
#include <memory>
#include <gmpxx.h>

#include <parser/verilog_number/verilog_number_scanner.hpp>

/// Generated by bison in the cmake build dir
#include <verilog_number/verilog_number_parser.hpp>


namespace Veriparse {
namespace Parser {

class VerilogNumberDriver {
public:
	VerilogNumberDriver(): m_parser(nullptr), m_scanner(nullptr) {}

	virtual ~VerilogNumberDriver();

	/**
	 * parse - parse from a c++ input stream
	 * @param is - std::istream&, valid input stream
	 */
	int parse(const std::string &str, mpz_class &mpz, int &size, int &base,
	          bool &is_signed, int &rc, std::string filename, int line);

private:
	int parse_helper(const std::string &str, mpz_class &mpz, int &size, int &base,
	                 bool &is_signed, int &rc, std::string filename, int line);

private:
	VerilogNumberParser  *m_parser;
	VerilogNumberScanner *m_scanner;

};

}
}

#endif
