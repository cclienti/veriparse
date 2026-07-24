// Type parameters (§6.20.3, §23.10): header formals with keyword and named
// defaults, a body localparam type, and instantiation actuals in keyword,
// identifier and ordered forms.
module tp_leaf #(parameter type T = logic [7:0], parameter type U = T,
                 parameter DEPTH = 4)
  (input T din, output T dout);

  localparam type acc_t = logic [15:0];
  parameter type sel_t = logic [1:0];

  T stage [DEPTH];
  acc_t acc;
  sel_t sel;

  assign dout = din;

endmodule

module sv_type_param0(input logic [15:0] a, output logic [15:0] b);

  typedef logic [15:0] word_t;

  tp_leaf #(.T(logic [15:0]), .DEPTH(8)) u0 (.din(a), .dout(b));
  tp_leaf #(.T(word_t)) u1 (.din(a), .dout());
  tp_leaf #(logic [15:0], word_t, 2) u2 (.din(a), .dout());

endmodule
