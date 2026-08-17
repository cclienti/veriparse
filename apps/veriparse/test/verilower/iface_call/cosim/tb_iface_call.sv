// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
// The interface-call differential proof: the behavioural machine calls
// bus.xfer() natively (Verilator runs the interface task), the lowered one
// holds the machine the splice-and-inline produced — no task, no call.
// Each takes its own instance of the same interface, with a symmetric ack
// responder reacting to that instance's own req, so both machines see the
// same environment shifted by the sampling skew. Registered outputs only:
// reference outputs compared through one posedge stage, $fatal on
// divergence (the iface_line convention).
module tb_iface_call;
   logic clk = 0, rst_n = 0;
   logic [1:0] phase_b, phase_l;
   logic [1:0] phase_b_q = '0;
   logic req_b_q = 0;
   logic [7:0] data_b_q = '0;
   int checked = 0;

   always #5 clk = ~clk;

   iface_call_bus bus_b(.clk(clk));
   iface_call_bus bus_l(.clk(clk));

   iface_call         u_beh (.rst_n(rst_n), .bus(bus_b), .phase(phase_b));
   iface_call_lowered u_low (.rst_n(rst_n), .bus(bus_l), .phase(phase_l));

   // One responder per instance: ack pulses one cycle after req rises and
   // clears while req holds, so every xfer takes at least one wait lap.
   always @(posedge clk) begin
      bus_b.ack <= bus_b.req & ~bus_b.ack;
      bus_l.ack <= bus_l.req & ~bus_l.ack;
   end

   always @(posedge clk) begin
      phase_b_q <= phase_b;
      req_b_q   <= bus_b.req;
      data_b_q  <= bus_b.data;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 300; k++) begin
         @(posedge clk);
         if (bus_l.req !== req_b_q || bus_l.data !== data_b_q || phase_l !== phase_b_q)
            $fatal(1, "DIVERGED t=%0t  req fsm=%b ref=%b  data fsm=%h ref=%h  phase fsm=%0d ref=%0d",
                   $time, bus_l.req, req_b_q, bus_l.data, data_b_q, phase_l, phase_b_q);
         checked++;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
