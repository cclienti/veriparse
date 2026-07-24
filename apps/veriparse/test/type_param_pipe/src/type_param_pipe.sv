// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti

// Type-parameterized accumulate/xor pipeline: the datapath widths come from
// `parameter type` overrides — a keyword type, a typedef actual, and the
// default — so the flattened widths are checkable against a C++ model.

module tpp_acc #(parameter type T = logic [7:0])
  (input logic clk, input logic rst, input T din, output T acc);

  T value;

  always @(posedge clk) begin
    if (rst) begin
      value <= T'(0);
    end else begin
      value <= T'(value + din);
    end
  end

  assign acc = value;

endmodule

module type_param_pipe (
    input logic clk,
    input logic rst,
    input logic [15:0] a,
    output logic [15:0] wide_acc,
    output logic [11:0] mid_acc,
    output logic [7:0] narrow_acc
);

  typedef logic [11:0] mid_t;

  tpp_acc #(.T(logic [15:0])) u_wide (.clk(clk), .rst(rst), .din(a), .acc(wide_acc));
  tpp_acc #(.T(mid_t)) u_mid (.clk(clk), .rst(rst), .din(a[11:0]), .acc(mid_acc));
  tpp_acc u_narrow (.clk(clk), .rst(rst), .din(a[7:0]), .acc(narrow_acc));

endmodule
