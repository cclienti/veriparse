#include <veriparse/parser/verilog_number.hpp>
#include <parser/verilog_number/verilog_number_driver.hpp>

#include <sstream>


namespace Veriparse {
	namespace Parser {

		VerilogNumber::VerilogNumber() {
			m_driver = new VerilogNumberDriver();
		}

		VerilogNumber::~VerilogNumber() {
			delete m_driver;
		}

		int VerilogNumber::parse(const std::string &str, mpz_class &mpz, int &size, int &base,
		                         bool &is_signed, int &rc, std::string filename, int line) {
			std::istringstream iss(str);
			return m_driver->parse(str, mpz, size, base, is_signed, rc, filename, line);
		}

	}
}
