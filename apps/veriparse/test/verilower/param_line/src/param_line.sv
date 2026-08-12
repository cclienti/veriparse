// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module param_line
  #(parameter int N = 4,
    parameter logic [7:0] SEED = 8'h11)
   (input logic clk, input logic rst_n,
    output logic [7:0] q, output logic done);

   (* veriparse_fsm *)
   initial begin
      q    <= '0;
      done <= 1'b0;
      @(posedge clk);
      q <= SEED;
      (* veriparse_no_unroll *)
      repeat (N) @(posedge clk);
      done <= 1'b1;
      @(posedge clk);
   end
endmodule
