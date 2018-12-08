#ifndef VERIPARSE_MISC_STRING_UTILS
#define VERIPARSE_MISC_STRING_UTILS

#include <vector>
#include <string>
#include <functional>

namespace Veriparse {
namespace Misc {
namespace StringUtils {

/**
 * If a string starts with '\\', an extra space character
 * will be appended in the returning string (if it does not
 * already exists in the given string).
 */
std::string escape(const std::string &str);

/**
 * Return a lowercased string from the given one.
 */
std::string lower(const std::string &str);

/**
 * Join a list in a string by applying the given function
 * "func" to all elements except for the last one. The
 * function "func_last" will be used for the last element.
 */
template<typename T, typename X> std::string join(const T &list,
                                                  std::function<std::string(X const&)> func,
                                                  std::function<std::string(X const&)> func_last) {
	std::string result;
	typename T::const_iterator it = list.begin();
	typename T::const_iterator end = list.end();
	typename T::const_iterator last = list.end();

	if (it != end) --last;

	for(; it != end ; ++it) {
		if (it == last) {
			result.append(func_last(*it));
		}
		else {
			result.append(func(*it));
		}
	}

	return result;
}

/**
 * Join an iterable list using the given separator (python-like).
 */
template<typename T>	std::string join(const std::string &separator, const T &list) {
	auto func = [&](const std::string &str){ return str + separator; };
	auto func_last = [](const std::string &str){ return str; };
	return join<T, std::string>(list, func, func_last);
}

/**
 * Remove the first surrounding brackets in a string. If
 * there is no brackets to remove, the new string is equal to
 * the given string.
 */
std::string delete_surrounding_brackets(const std::string &expression_str);

/**
 * Remove space characters in a string.
 */
std::string remove_spaces(const std::string &str);

/**
 * Remove space characters in a string.
 */
std::string remove_underscore(const std::string &str);

/**
 * Remove all whitespace characters (' ', '\f', '\n', '\r',
 * '\t', '\v') in a string.
 */
std::string remove_whitespace(const std::string &str);

/**
 * Remove all leading whitespace characters (' ', '\f', '\n',
 * '\r', '\t', '\v') in a string.
 */
std::string remove_leading_whitespace(const std::string &str);

/**
 * Remove all trailing whitespace characters (' ', '\f',
 * '\n', '\r', '\t', '\v') in a string.
 */
std::string remove_trailing_whitespace(const std::string &str);

/**
 * Remove all sub-string occurrences in the given string.
 */
std::string remove_all_sub_string(const std::string &str, const std::string &sub_str);

/**
 * Replace 'find' by 'replace' in the given string
 */
std::string replace_all(const std::string &str, const std::string &find, const std::string &replace);

/**
 * Remove trailing whitespace and the last ending semi-colon.
 */
std::string remove_last_semicolon(const std::string &str);

/**
 * Split a string using the given delim. It returns a vector
 * of string. A string in the vector can be empty.
 */
std::vector<std::string> split(const std::string &str, char delim);

/**
 * Indent a string using the given prefix. Trailing white
 * spaces will be removed.
 */
std::string indent(const std::string &str, const std::string &prefix="  ");

}
}
}

#endif
