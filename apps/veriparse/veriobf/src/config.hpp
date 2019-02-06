#ifndef VERIOBF_CONFIG_HPP
#define VERIOBF_CONFIG_HPP

#include <ostream>
#include <string>


struct Config
{
	std::string input;
	std::string output;
	std::uint64_t identifier_length;
	bool hash;
	std::uint64_t seed;
};

static inline std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "{"
		<< "input: '" << config.input << "', "
	   << "output: '" << config.output << "', "
	   << "identifier_length: " << config.identifier_length
	   << "random: " << config.hash
	   << "seed: " << config.seed
	   << "}";

	return os;
}


#endif
