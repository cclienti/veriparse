// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIOBF_CONFIG_HPP
#define VERIOBF_CONFIG_HPP

#include <ostream>
#include <cstdint>
#include <string>


struct Config
{
	std::string input;
	std::string output;
	std::uint64_t identifier_length;
	bool hash;
	bool sv_mode{false};
	std::uint64_t seed;
};

static inline std::ostream &operator<<(std::ostream &os, const Config &config)
{
	os << "{"
		<< "input: '" << config.input << "', "
	   << "output: '" << config.output << "', "
	   << "identifier_length: " << config.identifier_length << ", "
	   << "hash: " << config.hash << ", "
	   << "seed: " << config.seed
	   << "}";

	return os;
}


#endif
