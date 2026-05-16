// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module wire_split0
  (input clk,
   input [7:0] a,
   input [7:0] b,
   output [8:0] result);

   wire [7:0] x;
   wire [8:0] sum = {1'b0, a} + {1'b0, b};
   wire [7:0] y = a & b;
   wire [7:0] z = x | y;

   assign x = a ^ b;
   assign result = sum;

endmodule
