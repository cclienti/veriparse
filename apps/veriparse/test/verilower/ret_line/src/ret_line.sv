// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module ret_line
   (input  logic clk,
    input  logic rst_n,
    input  logic start,
    input  logic abort,
    input  logic [7:0] step,
    output logic [7:0] q,
    output logic busy,
    output logic done);

   task send(input logic [7:0] n = 8'd2);
      begin
         q <= q + n;
         @(posedge clk);
         if (abort) return;
         q <= q + n;
         @(posedge clk);
      end
   endtask

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
         send();
         send(step);
         busy <= 1'b0;
         done <= 1'b1;
         @(posedge clk);
      end
   end
endmodule
