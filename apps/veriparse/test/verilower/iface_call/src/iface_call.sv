// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
// A machine calling a task THROUGH its interface port (IEEE 1800-2017
// §25.7): the handshake helper lives with the bus it drives — it commits
// the bus's own members and waits on the bus's own clock — and the machine
// merely calls it. HierCallResolution splices the task into the module, the
// inlining stems BUS_XFER_0/BUS_XFER_1 name the per-site states, and the
// process clocks on the same clock member the spliced waits carry.
interface iface_call_bus(input logic clk);
   logic       req;
   logic       ack;
   logic [7:0] data;

   task xfer(input logic [7:0] n);
      begin
         data <= n;
         req  <= 1'b1;
         @(posedge clk);
         while (!ack) @(posedge clk);
         req  <= 1'b0;
         @(posedge clk);
      end
   endtask
endinterface

module iface_call
  (input  logic rst_n,
   iface_call_bus bus,
   output logic [1:0] phase);

   (* veriparse_fsm *)
   initial begin
      bus.req <= 1'b0;
      phase   <= 2'd0;
      @(posedge bus.clk);
      forever begin
         phase <= 2'd1;
         bus.xfer(8'h41);
         phase <= 2'd2;
         bus.xfer(8'h42);
         phase <= 2'd0;
         @(posedge bus.clk);
      end
   end
endmodule
