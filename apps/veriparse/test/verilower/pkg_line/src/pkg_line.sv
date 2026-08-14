// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
package pkg_line_lib;
   // A package body cannot see module scope (IEEE 1800-2017 §26.2), so the
   // machine reaches this task only through its formals: the clock arrives
   // by const ref — which makes the task automatic — and the count by
   // input, captured at entry (§12.7.3).
   task automatic hold_n(const ref logic ck, input logic [7:0] n);
      begin
         (* veriparse_no_unroll *)
         repeat (n) @(posedge ck);
      end
   endtask
endpackage

module pkg_line import pkg_line_lib::*;
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    input  logic [7:0] step,
    output logic [7:0] q,
    output logic busy,
    output logic done);

   (* veriparse_fsm *)
   initial begin
      q    <= '0;
      busy <= 1'b0;
      done <= 1'b0;
      @(posedge clk);
      forever begin
         done <= 1'b0;
         begin : ARMED
            while (!start) @(posedge clk);
         end
         busy <= 1'b1;
         @(posedge clk);
         hold_n(clk, step);
         q    <= q + step;
         busy <= 1'b0;
         done <= 1'b1;
         @(posedge clk);
      end
   end
endmodule
