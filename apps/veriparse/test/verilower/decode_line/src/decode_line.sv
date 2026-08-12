// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module decode_line
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    input  logic mode,
    output logic [7:0] q,
    output logic busy,
    output logic done,
    output logic sel);

   logic mode_r;

   (* veriparse_fsm *)
   initial begin
      q      <= '0;
      mode_r <= 1'b0;
      busy = 1'b0;
      done = 1'b0;
      sel  = 1'b0;
      @(posedge clk);
      forever begin
         busy = 1'b0;
         done = 1'b0;
         sel  = 1'b0;
         begin : ARMED
            while (!start) begin
               busy = 1'b0;
               done = 1'b0;
               sel  = 1'b0;
               mode_r <= mode;
               @(posedge clk);
            end
         end
         busy = 1'b1;
         done = 1'b0;
         if (mode_r) begin
            sel = 1'b1;
            q <= q + 8'd3;
         end else begin
            sel = 1'b0;
            q <= q + 8'd1;
         end
         @(posedge clk);
         busy = 1'b1;
         done = 1'b0;
         sel  = 1'b0;
         q <= q ^ 8'h55;
         @(posedge clk);
         busy = 1'b0;
         done = 1'b1;
         sel  = 1'b0;
         @(posedge clk);
      end
   end
endmodule
