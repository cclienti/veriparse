#ifndef VERIPARSE_PARSER_VERILOG
#define VERIPARSE_PARSER_VERILOG

#include <iostream>
#include <string>
#include <veriparse/AST/nodes.hpp>

namespace Veriparse {
	namespace Parser {

		class VerilogDriver;

		class Verilog {
		public:

			Verilog();

			virtual ~Verilog();

			/**
			 * parse - parse from a file
			 * @param filename - valid string with input file
			 */
			int parse(const std::string &filename);

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

	}
}

#endif
