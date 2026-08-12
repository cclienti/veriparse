// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module pulse_ctr
  #(parameter int N = 4)
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    output logic busy,
    output logic done);

   (* veriparse_fsm *)
   initial begin
      busy <= 1'b0;
      done <= 1'b0;
      @(posedge clk);
      forever begin
         done <= 1'b0;
         begin : ARMED
            while (!start) @(posedge clk);
         end
         busy <= 1'b1;
         begin : COUNT
            (* veriparse_no_unroll *)
            repeat (N) @(posedge clk);
         end
         busy <= 1'b0;
         done <= 1'b1;
         @(posedge clk);
      end
   end
endmodule

module fsm_flat
   (input  logic       clk,
    input  logic       rst_n,
    input  logic       start,
    output logic [1:0] busy,
    output logic [1:0] done);

   pulse_ctr #(.N(3)) u_fast (.clk(clk), .rst_n(rst_n), .start(start),
                              .busy(busy[0]), .done(done[0]));
   pulse_ctr #(.N(6)) u_slow (.clk(clk), .rst_n(rst_n), .start(start),
                              .busy(busy[1]), .done(done[1]));
endmodule
