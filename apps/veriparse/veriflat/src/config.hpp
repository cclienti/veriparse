#ifndef VERIFLAT_CONFIG_HPP
#define VERIFLAT_CONFIG_HPP

#include <vector>
#include <ostream>
#include <sstream>
#include <string>
#include <cstdint>

struct Config
{
	std::vector<std::string> inputs;
	std::string output;
	std::string top_module;
	std::string param_map;
	bool deadcode_at_end{false};
	bool deadcode_during_flatten{false};
	std::uint64_t seed;
};

static inline std::ostream &operator<<(std::ostream &os, const Config &config)
{
	std::stringstream ss;
	for (int i = config.inputs.size()-1; i >= 0; i--) {
		ss << "'" << config.inputs[i] << "'";
		if (i != 0) {
			ss << ", ";
		}
	}

	os << "{"
	   << "inputs: [" << ss.str() << "], "
	   << "output: '" << config.output << "', "
	   << "top_module: '" << config.top_module << "', "
	   << "param_map: " << config.param_map << ", "
	   << "seed: " << config.seed
	   << "}";

	return os;
}


#endif
