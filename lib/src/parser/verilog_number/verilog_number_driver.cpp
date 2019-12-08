#include <cctype>
#include <sstream>
#include <cassert>

#include "./verilog_number_driver.hpp"

namespace Veriparse {
namespace Parser {

VerilogNumberDriver::~VerilogNumberDriver() {
	delete(m_scanner);
	m_scanner = nullptr;
	delete(m_parser);
	m_parser = nullptr;
}

int VerilogNumberDriver::parse(const std::string &str, mpz_class &mpz, int &size, int &base,
                               bool &is_signed, int &rc, std::string filename, int line) {
	return parse_helper(str, mpz, size, base, is_signed, rc, filename, line);
}

int VerilogNumberDriver::parse_helper(const std::string &str, mpz_class &mpz, int &size, int &base,
                                      bool &is_signed, int &rc, std::string filename, int line) {
	std::istringstream iss(str);

	delete(m_scanner);
	try {
		m_scanner = new VerilogNumberScanner(&iss);
	}
	catch(std::bad_alloc &ba) {
		std::cerr << "Failed to allocate scanner: ("
		          << ba.what() << "), exiting!!\n";
		return 2;
	}

	delete(m_parser);
	try {
		m_parser = new VerilogNumberParser(*m_scanner, *this, str, mpz, size, base, is_signed, rc, filename, line);
	}
	catch(std::bad_alloc &ba) {
		std::cerr << "Failed to allocate parser: ("
		          << ba.what() << "), exiting!!\n";
		return 2;
	}

	int ret = m_parser->parse();

	if (ret != 0) {
		std::cerr << "Parse failed!!\n";
		return ret;
	}

	return 0;
}

}
}
