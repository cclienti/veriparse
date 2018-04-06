#ifndef VERIPARSE_PARSER_VERILOG_NUMBER
#define VERIPARSE_PARSER_VERILOG_NUMBER

#include <iostream>
#include <string>
#include <gmpxx.h>

namespace Veriparse {
	namespace Parser {

		class VerilogNumberDriver;

		class VerilogNumber {
		public:

			VerilogNumber();

			virtual ~VerilogNumber();

			/**
			 * @brief parse a verilog number
			 * @param str, verilog number string to parse
			 * @param mpz, multiprecision parsed number
			 * @param size, width of the parsed number (-1 if not applicable)
			 * @param base, base used to declare the number
			 * @param is_signed, set to true if the number is declared as a signed number
			 * @param rc, return to code that indicates if the mpz number is valid
			 * @param filename, filename where the string to parse is declared
			 * @param line, line number where the string to parse is declared
			 * @return 0 on success.
			 */
			int parse(const std::string &str, mpz_class &mpz, int &size, int &base,
			          bool &is_signed, int &rc, std::string filename="", int line=0);

		private:
			VerilogNumberDriver *m_driver = nullptr;
		};

	}
}

#endif
