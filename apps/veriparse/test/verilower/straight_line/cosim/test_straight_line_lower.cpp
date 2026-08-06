// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// The verilower output: one reset cycle puts every register where time zero
// left the reference (ADR-0014 §5.1), so the same trace holds at an offset
// equal to the hold — the §11 alignment, with the hold at its minimum of
// one sampled edge.

#include "expected_trace.hpp"

#include "Vstraight_line.h"

TEST(VerilowerStraightLineCosim, LoweredMachine)
{
    Vstraight_line dut;
    check_from_reset(dut, 1);
}
