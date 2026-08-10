// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERILOWER_REPORT_HPP
#define VERILOWER_REPORT_HPP

#include <veriparse/passes/transformations/implicit_fsm_elaboration.hpp>
#include <cstdio>
#include <sstream>
#include <string>

/// JSON string escaping for the few characters a rendered guard can carry.
static inline std::string json_escape(const std::string &str)
{
    std::string out;
    for(const char c : str) {
        switch(c) {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\t':
            out += "\\t";
            break;
        case '\r':
            out += "\\r";
            break;
        default:
            if(static_cast<unsigned char>(c) < 0x20) {
                char buffer[8];
                std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                out += buffer;
            } else {
                out += c;
            }
        }
    }
    return out;
}

/// The §10.2 state map: per process the state variable, its width and
/// encoding, the reset contract, and per state its name, encoded value and
/// source line. JSON is the canonical form; everything else derives.
static inline std::string render_state_map(
    const Veriparse::Passes::Transformations::ImplicitFsmElaboration::FsmReport &report)
{
    std::stringstream json;
    json << "{\n  \"processes\": [";
    bool first_process = true;
    for(const auto &process : report.processes) {
        json << (first_process ? "" : ",") << "\n    {\n";
        first_process = false;
        json << "      \"module\": \"" << json_escape(process.module_name) << "\",\n";
        json << "      \"state_variable\": \"" << json_escape(process.state_variable) << "\",\n";
        json << "      \"width\": " << process.width << ",\n";
        json << "      \"encoding\": \"" << process.encoding << "\",\n";
        json << "      \"entry\": \"" << json_escape(process.entry) << "\",\n";
        json << "      \"reset\": {\n";
        json << "        \"signal\": \"" << json_escape(process.reset_signal) << "\",\n";
        json << "        \"active_level\": " << process.reset_active_level << ",\n";
        json << "        \"kind\": \"" << process.reset_kind << "\",\n";
        json << "        \"registers\": [";
        for(std::size_t i = 0; i < process.reset_registers.size(); ++i) {
            json << (i ? ", " : "") << "\"" << json_escape(process.reset_registers[i]) << "\"";
        }
        json << "]\n      },\n";
        json << "      \"states\": [";
        for(std::size_t i = 0; i < process.states.size(); ++i) {
            const auto &state = process.states[i];
            json << (i ? "," : "") << "\n        {\"name\": \"" << json_escape(state.name)
                 << "\", \"value\": " << state.value << ", \"line\": " << state.line << "}";
        }
        json << "\n      ],\n";
        json << "      \"transitions\": [";
        for(std::size_t i = 0; i < process.transitions.size(); ++i) {
            const auto &edge = process.transitions[i];
            json << (i ? "," : "") << "\n        {\"from\": \"" << json_escape(edge.from)
                 << "\", \"to\": \"" << json_escape(edge.to) << "\", \"guard\": \""
                 << json_escape(edge.guard) << "\", \"action\": \"" << json_escape(edge.action)
                 << "\"}";
        }
        json << "\n      ]\n    }";
    }
    json << "\n  ]\n}\n";
    return json.str();
}

/// Escape for a double-quoted dot string.
static inline std::string dot_escape(const std::string &str)
{
    std::string out;
    for(const char c : str) {
        if(c == '"' || c == '\\') {
            out += '\\';
        }
        out += c;
    }
    return out;
}

/// The graphviz view, printed from the same report as the JSON: states as
/// circles, the reset entry as a double circle, guards on the edges — and
/// the register updates only when asked, so the picture stays a state
/// graph, not a listing.
static inline std::string
render_fsm_dot(const Veriparse::Passes::Transformations::ImplicitFsmElaboration::FsmReport &report,
               bool with_values)
{
    std::stringstream dot;
    // One digraph, one cluster per compiled process: state names are
    // prefix-qualified and prefixes are distinct per process, so the node
    // namespaces cannot collide.
    dot << "digraph fsm {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=circle];\n";
    std::size_t index = 0;
    for(const auto &process : report.processes) {
        dot << "  subgraph \"cluster_" << index++ << "\" {\n";
        dot << "    label=\"" << dot_escape(process.module_name) << " / "
            << dot_escape(process.state_variable) << "\";\n";
        dot << "    \"" << dot_escape(process.entry) << "\" [shape=doublecircle];\n";
        for(const auto &edge : process.transitions) {
            dot << "    \"" << dot_escape(edge.from) << "\" -> \"" << dot_escape(edge.to) << "\"";
            std::string label = edge.guard;
            if(with_values && !edge.action.empty()) {
                label += label.empty() ? edge.action : " / " + edge.action;
            }
            if(!label.empty()) {
                dot << " [label=\"" << dot_escape(label) << "\"]";
            }
            dot << ";\n";
        }
        dot << "  }\n";
    }
    dot << "}\n";
    return dot.str();
}

#endif
