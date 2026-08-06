// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// The behavioural reference, verilated under --timing: the marked initial
// runs as written, from time zero, ignoring rst_n (ADR-0014 §5.2) — so the
// trace is checked at zero hold.

#include "expected_trace.hpp"

#include "Vstraight_line.h"

TEST(VerilowerStraightLineCosim, BehavioralReference)
{
    Vstraight_line dut;
    check_from_reset(dut, 0);
}
