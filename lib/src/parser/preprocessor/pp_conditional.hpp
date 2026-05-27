// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
//
// Derived from IEEE Std 1800-2017 §22.6 and IEEE Std 1364-2005 §19.4.
#ifndef VERIPARSE_PREPROCESSOR_CONDITIONAL_HPP
#define VERIPARSE_PREPROCESSOR_CONDITIONAL_HPP

#include <string>
#include <vector>

namespace Veriparse
{
namespace Parser
{

/// Per-frame state for an open `ifdef/`ifndef block.
///   Taken           — this branch is being emitted
///   SkippingLooking — this branch is skipped, a later `elsif/`else may
///                     still be taken
///   SkippingDone    — a branch in this block was already taken (or the
///                     parent frame is itself skipping); every remaining
///                     branch up to `endif is skipped
enum class CondState
{
    Taken,
    SkippingLooking,
    SkippingDone
};

struct CondFrame
{
    CondState state;
    bool in_else{false};
    std::string opened_filename;
    int opened_line{0};
};

class ConditionalStack
{
public:
    /// `ifdef NAME — defined controls whether NAME is in the macro table.
    /// (The caller does the table lookup so this class stays free of
    /// macro-table coupling.)
    void push_ifdef(bool defined, const std::string &filename, int line);

    /// `ifndef NAME — same as push_ifdef with inverted defined flag.
    void push_ifndef(bool defined, const std::string &filename, int line);

    /// `elsif NAME — returns false if the directive is misplaced (e.g.
    /// after `else, or with no open `ifdef).
    bool handle_elsif(bool defined, const std::string &filename, int line);

    /// `else — returns false if misplaced.
    bool handle_else(const std::string &filename, int line);

    /// `endif — returns false if there is no open `ifdef.
    bool handle_endif(const std::string &filename, int line);

    /// True iff every frame on the stack is Taken (or the stack is empty).
    bool emitting() const;

    bool empty() const { return m_stack.empty(); }
    const CondFrame &top() const { return m_stack.back(); }

private:
    std::vector<CondFrame> m_stack;
};

} // namespace Parser
} // namespace Veriparse

#endif
