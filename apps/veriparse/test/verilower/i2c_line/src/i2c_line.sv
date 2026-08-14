// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module i2c_line
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    input  logic [7:0] byte_in,
    output logic scl,
    output logic sda,
    output logic busy,
    output logic done);

   logic [3:0] nbit;

   task phase(input logic v, input logic [7:0] hold);
      begin
         scl <= v;
         (* veriparse_no_unroll *)
         repeat (hold) @(posedge clk);
      end
   endtask

   (* veriparse_fsm *)
   initial begin
      scl  <= 1'b1;
      sda  <= 1'b1;
      busy <= 1'b0;
      done <= 1'b0;
      nbit <= 4'd0;
      @(posedge clk);
      forever begin
         done <= 1'b0;
         begin : ARMED
            while (!start) @(posedge clk);
         end
         busy <= 1'b1;
         sda  <= 1'b0;
         nbit <= 4'd8;
         phase(1'b1, 8'd2);
         begin : BIT
            while (nbit != 0) begin
               sda  <= byte_in[nbit-4'd1];
               nbit <= nbit - 4'd1;
               phase(1'b0, 8'd2);
               phase(1'b1, 8'd2);
            end
         end
         sda <= 1'b0;
         phase(1'b0, 8'd2);
         phase(1'b1, 8'd2);
         sda  <= 1'b1;
         busy <= 1'b0;
         done <= 1'b1;
         @(posedge clk);
      end
   end
endmodule
