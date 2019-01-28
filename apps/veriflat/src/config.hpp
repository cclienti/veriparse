#ifndef VERIFLAT_CONFIG_HPP
#define VERIFLAT_CONFIG_HPP

#include <vector>
#include <ostream>
#include <sstream>
#include <string>


struct Config
{
	std::vector<std::string> inputs;
	std::string output;
	std::string top_module;
	std::string param_map;
	bool obfuscate {false};
};

static inline std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "{inputs: [";
	std::stringstream ss;
	for (int i = config.inputs.size()-1; i >= 0; i--) {
		ss << "'" << config.inputs[i] << "'";
		if (i != 0) {
			ss << ", ";
		}
	}
	os << ss.str() << "], ";

	os << "output: '" << config.output << "', ";
	os << "top_module: '" << config.top_module << "', ";
	os << "param_map: " << config.param_map << ", ";
	os << "obfuscate: " << config.obfuscate;
	os << "}";

	return os;
}


#endif
