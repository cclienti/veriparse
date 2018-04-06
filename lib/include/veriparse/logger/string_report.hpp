#ifndef VERIPARSE_LOGGER_STRING_REPORT_HPP
#define VERIPARSE_LOGGER_STRING_REPORT_HPP

#include <string>

namespace Veriparse {
	namespace Logger {

		/**
		 * Return a string that contains the error message.
		 *
		 * The location is described by begin_col and end_col.
		 *
		 * The context describes how many lines must be printed before
		 * and after the error.
		 */
		std::string string_report(const std::string &err_message, const std::string &str, int begin_col, int end_col);

	}
}

#endif
