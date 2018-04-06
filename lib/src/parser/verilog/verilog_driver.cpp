#include <cctype>
#include <fstream>
#include <cassert>

#include "./verilog_driver.hpp"

namespace Veriparse {
	namespace Parser {

		VerilogDriver::~VerilogDriver() {
			delete(scanner);
			scanner = nullptr;
			delete(parser);
			parser = nullptr;
		}

		int VerilogDriver::parse(const std::string &filename) {
			std::ifstream in_file(filename);
			if(!in_file.good())	{
				LOG_ERROR << "Could not read \"" << filename << "\"";
				return 1;
			}

			return parse_helper(in_file, filename);
		}

		int VerilogDriver::parse(std::istream &stream) {
			if(! stream.good() && stream.eof()) {
				LOG_ERROR << "Could not read the stream to parse";
				return 1;
			}

			return parse_helper(stream);
		}

		int VerilogDriver::parse_helper(std::istream &stream, const std::string &filename) {
			delete(scanner);
			try {
				scanner = new VerilogScanner(&stream, filename);
			}
			catch(std::bad_alloc &ba) {
				LOG_FATAL << "Failed to allocate scanner: ("
				          << ba.what() << "), exiting!!\n";
				exit(EXIT_FAILURE);
			}

			delete(parser);
			try {
				parser = new VerilogParser((*scanner),
				                           (*this));
			}
			catch(std::bad_alloc &ba) {
				LOG_FATAL << "Failed to allocate parser: ("
				          << ba.what() << "), exiting!!\n";
				exit(EXIT_FAILURE);
			}

			if(parser->parse() != 0) {
				LOG_ERROR << "Parse failed!\n";
				return 1;
			}

			return 0;
		}

	}
}
