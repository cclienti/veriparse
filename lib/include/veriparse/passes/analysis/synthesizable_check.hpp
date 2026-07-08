// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#ifndef VERIPARSE_PASSES_ANALYSIS_SYNTHESIZABLE_CHECK
#define VERIPARSE_PASSES_ANALYSIS_SYNTHESIZABLE_CHECK

#include <vector>

#include <veriparse/AST/nodes.hpp>

namespace Veriparse
{
namespace Passes
{
namespace Analysis
{

/**
 * @brief Reject constructs outside the synthesizable RTL subset.
 *
 * The parser accepts a broader slice of SystemVerilog than the transformation
 * passes can lower to RTL, so some legal, round-trippable syntax has no meaning
 * for the flattener. This pass is the single place that draws that line: it
 * walks the design and reports every construct that is parsed but not
 * synthesizable. It is a blacklist (reject a known set), not a whitelist —
 * additive as more non-synthesizable grammar lands.
 *
 * It mutates nothing and is opt-in per tool: veriflat runs it (its output must
 * be synthesizable), veridump/veriobf do not.
 */
class SynthesizableCheck
{
public:
    SynthesizableCheck() = delete;

    /**
     * @brief Check a single design tree.
     *
     * Reports every non-synthesizable construct via the logger.
     *
     * @return the number of violations found (0 ⇒ synthesizable).
     */
    static int check(const AST::Node::Ptr &node);

    /**
     * @brief Check a design spread over several trees (one per compilation unit).
     *
     * @return the total number of violations across every tree.
     */
    static int check(const std::vector<AST::Node::Ptr> &sources);
};

} // namespace Analysis
} // namespace Passes
} // namespace Veriparse

#endif
