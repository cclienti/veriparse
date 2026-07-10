// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Parameterized valid/ready handshake over a SystemVerilog interface:
// modports, interface body logic, per-instance parameterization, and a
// 4-lane interface array (arrayed child instances + an arrayed interface
// port).

interface hs_if #(parameter W = 8);
  logic         valid;
  logic         ready;
  logic [W-1:0] data;
  logic         fire;

  assign fire = valid & ready;

  modport src(output valid, output data, input ready, input fire);
  modport dst(input valid, input data, output ready, input fire);
endinterface

module hs_producer #(parameter W = 8) (hs_if.src p, input logic clk, input logic rst);

  logic [W-1:0] counter;

  always @(posedge clk) begin
    if (rst) begin
      counter <= {W{1'b0}};
    end else if (p.fire) begin
      counter <= counter + 1'b1;
    end
  end

  assign p.valid = 1'b1;
  assign p.data = counter;

endmodule

module hs_consumer #(parameter W = 8) (hs_if.dst p, input logic clk, input logic rst,
                                       output logic [W-1:0] acc, output logic [7:0] count);

  logic toggle;

  always @(posedge clk) begin
    if (rst) begin
      toggle <= 1'b0;
      acc <= {W{1'b0}};
      count <= 8'd0;
    end else begin
      toggle <= ~toggle;
      if (p.fire) begin
        acc <= acc ^ p.data;
        count <= count + 8'd1;
      end
    end
  end

  assign p.ready = toggle;

endmodule

module lane_src (hs_if.src p, input logic [7:0] seed);

  assign p.valid = 1'b1;
  assign p.data = seed;

endmodule

module lane_mix (hs_if.dst p[3:0], output logic [7:0] mix);

  assign p[0].ready = 1'b1;
  assign p[1].ready = 1'b1;
  assign p[2].ready = 1'b1;
  assign p[3].ready = 1'b1;

  assign mix = p[0].data ^ p[1].data ^ p[2].data ^ p[3].data;

endmodule

module iface_handshake (
    input logic clk,
    input logic rst,
    input logic [31:0] seeds,
    output logic [15:0] acc,
    output logic [7:0] count,
    output logic [7:0] mix
);

  hs_if #(.W(16)) bus();

  hs_producer #(.W(16)) u_prod(.p(bus), .clk(clk), .rst(rst));
  hs_consumer #(.W(16)) u_cons(.p(bus), .clk(clk), .rst(rst), .acc(acc), .count(count));

  hs_if #(.W(8)) lanes[3:0]();

  lane_src u_src[3:0](.p(lanes), .seed(seeds));
  lane_mix u_mix(.p(lanes), .mix(mix));

endmodule
