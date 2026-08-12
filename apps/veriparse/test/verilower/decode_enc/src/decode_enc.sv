// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module decode_enc
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    output logic [7:0] q,
    output logic busy,
    output logic done);

   (* veriparse_fsm, veriparse_encoding = "output" *)
   initial begin
      q    <= '0;
      busy = 1'b0;
      done = 1'b0;
      @(posedge clk);
      forever begin
         busy = 1'b0;
         done = 1'b0;
         begin : ARMED
            while (!start) begin
               busy = 1'b0;
               done = 1'b0;
               @(posedge clk);
            end
         end
         busy = 1'b1;
         done = 1'b0;
         q <= q + 8'd1;
         @(posedge clk);
         busy = 1'b1;
         done = 1'b0;
         q <= q ^ 8'h55;
         @(posedge clk);
         busy = 1'b0;
         done = 1'b1;
         @(posedge clk);
      end
   end
endmodule
