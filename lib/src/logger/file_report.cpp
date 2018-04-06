#include <veriparse/logger/file_report.hpp>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/misc/math.hpp>

#include <fstream>
#include <sstream>
#include <iomanip>


namespace Veriparse {
	namespace Logger {

		std::string file_report(const std::string &err_message, const std::string &filename,
		                        int begin_line, int end_line, int begin_col, int end_col, int context) {

			std::stringstream ss;
			std::string s;
			std::ifstream f(filename);

			if (!f) {
				return std::string("Could not open \"") + filename + "\"";
			}

			// How many lines to get before and after the error
			context = (context <= 0) ? 1 : context;

			// Error prologue
			ss << "in file \"" << filename << "\", ";

			if (begin_line == end_line) {
				ss << "line " << begin_line << ": ";
			}else {
				ss << "line " << begin_line << " to " << end_line << ": ";
			}

			std::string errmsg = Misc::StringUtils::replace_all(err_message, "\"'", "\"");
			ss << Misc::StringUtils::replace_all(errmsg, "'\"", "\"");
			ss << ":";

			// Seek to the line before the error
			int startl = (begin_line < context) ? 1 : (begin_line - context);
			for (int i=1; i<startl; i++) std::getline(f, s);

			int dstart = Misc::Math::log10(startl) + 2;
			int dend = Misc::Math::log10(end_line + context) + 2;
			int dwidth = dstart > dend ? dstart : dend;

			// Print context before
			for (int i=begin_line-context; i<begin_line; i++) {
				if (i<1) continue;
				if (std::getline(f, s)) {
					ss << "\n" << std::setw(dwidth) << i << std::setw(0) << ":\t" << s;
				}
			}

			// Print the error
			for (int i=begin_line; i<=end_line; i++) {
				if(std::getline(f, s)) {

					ss << "\n" << std::setw(dwidth) << i << std::setw(0) << ":\t" << s << "\n\t";

					for (int j=1; j<=static_cast<int>(s.size()); j++) {
						if ((i==begin_line) && (j < begin_col)) {
							ss << " ";
						}
						else if (i < end_line) {
							ss << "^";
						}
						else if ((i == end_line) && (j < end_col)) {
							ss << "^";
						}
					}

				}
			}

			// Print the context after
			for (int i=(end_line+1); i<(end_line + context + 1); i++) {
				if (std::getline(f, s)) {
					ss << "\n" << std::setw(dwidth) << i << std::setw(0) << ":\t" << s;
				}
			}

			return ss.str();
		}

	}
}
