// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module flat_param
  #(parameter int N = 4,
    parameter logic [7:0] SEED = 8'h11)
   (input  logic         clk,
    input  logic [N-1:0] d,
    output logic [N-1:0] q);

   always_ff @(posedge clk) q <= d ^ SEED[N-1:0];
endmodule
