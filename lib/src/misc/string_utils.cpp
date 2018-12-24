#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

#include <veriparse/misc/string_utils.hpp>


namespace Veriparse {
namespace Misc {
namespace StringUtils {

std::string escape(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);

	if (result.front() != '\\') {
		return result;
	}

	if (result.back() != ' ') {
		result.push_back(' ');
	}

	return result;
}

std::string lower(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string delete_surrounding_brackets(const std::string &expression_str) {
	if (expression_str.size() == 0) {
		return expression_str;
	}

	std::string result(expression_str);
	if ((result.front() == '(') && (result.back() == ')')) {
		result = result.substr(1, result.size() - 2);
	}
	return result;
}

std::string remove_spaces(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
	return result;
}

std::string remove_underscore(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	result.erase(std::remove(result.begin(), result.end(), '_'), result.end());
	return result;
}

std::string remove_whitespace(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
	return result;
}

std::string remove_leading_whitespace(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	result.erase(result.begin(), find_if_not(result.begin(), result.end(), [](int c) {return isspace(c);}));
	return result;
}

std::string remove_trailing_whitespace(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	result.erase(find_if_not(result.rbegin(), result.rend(), [](int c) {return isspace(c);}).base(), result.end());
	return result;
}

std::string remove_last_semicolon(const std::string &str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result = remove_trailing_whitespace(str);
	if (result.back() == ';') {
		return result.substr(0, result.size() - 1);
	}
	return result;
}

std::string remove_all_sub_string(const std::string &str, const std::string &sub_str) {
	if (str.size() == 0) {
		return str;
	}

	std::string result(str);
	std::string::size_type n = sub_str.length();

	if (n==0) return result;

	for (std::string::size_type i = result.find(sub_str); i != std::string::npos; i = result.find(sub_str)) {
		result.erase(i, n);
	}

	return result;
}

std::string replace_all(const std::string &str, const std::string &find, const std::string &replace) {
	if (str.size() == 0) {
		return str;
	}

	std::string result = str;
	for(std::string::size_type i = 0; (i=result.find(find, i)) != std::string::npos;) {
		result.replace(i, find.length(), replace);
		i += replace.length();
	}
	return result;
}

std::vector<std::string> split(const std::string &str, char delim) {
	std::vector<std::string> result;

	if (str.size() == 0) {
		return result;
	}

	std::stringstream ss;
	std::string item;

	ss.str(str);
	while (std::getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

std::string indent(const std::string &str, const std::string &prefix) {
	if (str.size() == 0) {
		return str;
	}

	std::vector<std::string> splitted = split(str, '\n');
	std::string result;

	bool first = true;

	for(std::string &line : splitted) {
		if (first) first = false;
		else result.append("\n");

		std::string trimed = remove_trailing_whitespace(line);
		if (!trimed.empty()) {
			result.append(prefix);
			result.append(trimed);
		}
	}

	return result;
}

}
}
}
