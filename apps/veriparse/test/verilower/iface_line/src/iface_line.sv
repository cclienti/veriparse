// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
// A machine whose ports are an interface: the interface is kept as it
// stands — never inlined — and its members are signals of the machine,
// read in guards and committed with '<=' through their whole path (IEEE
// 1800-2017 §25.3). One member travels through a task formal as an input
// actual, and the task body commits another through its whole path, so the
// inlining carries the paths as they stand (§13.3).
interface iface_line_bus;
   logic       req;
   logic [7:0] data;
   logic       ack;
   logic [7:0] sum;

   modport dev(input req, input data, output ack, output sum);
endinterface

module iface_line
  (input  var logic clk,
   input  logic rst_n,
   iface_line_bus.dev bus,
   output logic busy);

   task accumulate(input logic [7:0] n);
      begin
         @(posedge clk);
         bus.sum <= bus.sum + n;
         @(posedge clk);
      end
   endtask

   (* veriparse_fsm *)
   initial begin
      bus.ack <= 1'b0;
      bus.sum <= '0;
      busy    <= 1'b0;
      @(posedge clk);
      forever begin
         bus.ack <= 1'b0;
         while (!bus.req) @(posedge clk);
         busy <= 1'b1;
         accumulate(bus.data);
         bus.ack <= 1'b1;
         busy    <= 1'b0;
         @(posedge clk);
      end
   end
endmodule
