// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
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
    std::vector<std::string> include_dirs;
    std::vector<std::string> defines;
    std::vector<std::string> undefs;
    std::string output;
    std::string top_module;
    std::string param_map;
    bool deadcode_at_end{false};
    bool deadcode_during_flatten{false};
    bool sv_mode{false};
    std::uint64_t seed;
};

static inline std::ostream &operator<<(std::ostream &os, const Config &config)
{
    std::stringstream ss;
    for(int i = config.inputs.size() - 1; i >= 0; i--) {
        ss << "'" << config.inputs[i] << "'";
        if(i != 0) {
            ss << ", ";
        }
    }

    os << "{"
       << "inputs: [" << ss.str() << "], "
       << "output: '" << config.output << "', "
       << "top_module: '" << config.top_module << "', "
       << "param_map: " << config.param_map << ", "
       << "seed: " << config.seed << "}";

    return os;
}

#endif
