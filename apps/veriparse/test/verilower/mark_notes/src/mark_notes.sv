// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
module gen_marked (input logic clk, input logic rst_n, output logic done);
   generate
      if (1) begin : g
         (* veriparse_fsm *)
         initial begin
            done <= 1'b0;
            @(posedge clk);
            done <= 1'b1;
            @(posedge clk);
         end
      end
   endgenerate
endmodule

module mark_notes (input logic a, output logic y);
   assign y = a;
endmodule
