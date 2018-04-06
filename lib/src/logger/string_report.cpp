#include <veriparse/logger/file_report.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/misc/math.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>


namespace Veriparse {
	namespace Logger {

		std::string string_report(const std::string &err_message, const std::string &str, int begin_col, int end_col) {

			std::stringstream ss;
			std::string s;

			// Error prologue
			ss << "error in \"" << str << "\", column " << begin_col;
			if (begin_col < end_col) {
				ss << " to " << end_col;
			}

			ss << ": " << err_message;

			return ss.str();
		}

	}
}
