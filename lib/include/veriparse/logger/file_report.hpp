#ifndef VERIPARSE_LOGGER_FILE_REPORT_HPP
#define VERIPARSE_LOGGER_FILE_REPORT_HPP

#include <string>

namespace Veriparse {
	namespace Logger {

		/**
		 * Return a string that contains the error message followed by
		 * the highlighted content around the error location in the
		 * given file.
		 *
		 * The location is described by (begin_line, begin_col) and
		 * (end_line, end_col).
		 *
		 * The context describes how many lines must be printed before
		 * and after the error.
		 */
		std::string file_report(const std::string &err_message, const std::string &filename,
		                        int begin_line, int end_line, int begin_col, int end_col, int context=1);

	}
}

#endif
