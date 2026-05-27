// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

#include <parser/preprocessor/pp_conditional.hpp>
#include <veriparse/logger/logger.hpp>

namespace Veriparse
{
namespace Parser
{

namespace
{

/// When opening a nested frame inside a parent that is already skipping,
/// every branch is dead regardless of the macro check — collapse to
/// SkippingDone so `elsif/`else cannot accidentally activate it.
CondState frame_state_for_new_block(bool taken_now, bool parent_emitting)
{
    if(!parent_emitting) {
        return CondState::SkippingDone;
    }
    return taken_now ? CondState::Taken : CondState::SkippingLooking;
}

} // namespace

void ConditionalStack::push_ifdef(bool defined, const std::string &filename, int line)
{
    const bool parent_emitting = emitting();
    m_stack.push_back(
        CondFrame{frame_state_for_new_block(defined, parent_emitting), false, filename, line});
}

void ConditionalStack::push_ifndef(bool defined, const std::string &filename, int line)
{
    push_ifdef(!defined, filename, line);
}

bool ConditionalStack::handle_elsif(bool defined, const std::string &filename, int line)
{
    if(m_stack.empty()) {
        LOG_ERROR << "file \"" << filename << "\", line " << line
                  << ": `elsif with no matching `ifdef/`ifndef";
        return false;
    }
    CondFrame &top = m_stack.back();
    if(top.in_else) {
        LOG_ERROR << "file \"" << filename << "\", line " << line
                  << ": `elsif after `else (opened at \"" << top.opened_filename << "\", line "
                  << top.opened_line << ")";
        return false;
    }
    switch(top.state) {
    case CondState::Taken:
        top.state = CondState::SkippingDone;
        break;
    case CondState::SkippingLooking:
        top.state = defined ? CondState::Taken : CondState::SkippingLooking;
        break;
    case CondState::SkippingDone:
        break;
    }
    return true;
}

bool ConditionalStack::handle_else(const std::string &filename, int line)
{
    if(m_stack.empty()) {
        LOG_ERROR << "file \"" << filename << "\", line " << line
                  << ": `else with no matching `ifdef/`ifndef";
        return false;
    }
    CondFrame &top = m_stack.back();
    if(top.in_else) {
        LOG_ERROR << "file \"" << filename << "\", line " << line
                  << ": duplicate `else (opened at \"" << top.opened_filename << "\", line "
                  << top.opened_line << ")";
        return false;
    }
    top.in_else = true;
    switch(top.state) {
    case CondState::Taken:
        top.state = CondState::SkippingDone;
        break;
    case CondState::SkippingLooking:
        top.state = CondState::Taken;
        break;
    case CondState::SkippingDone:
        break;
    }
    return true;
}

bool ConditionalStack::handle_endif(const std::string &filename, int line)
{
    if(m_stack.empty()) {
        LOG_ERROR << "file \"" << filename << "\", line " << line
                  << ": `endif with no matching `ifdef/`ifndef";
        return false;
    }
    m_stack.pop_back();
    return true;
}

bool ConditionalStack::emitting() const
{
    for(const auto &f : m_stack) {
        if(f.state != CondState::Taken) {
            return false;
        }
    }
    return true;
}

} // namespace Parser
} // namespace Veriparse
