// Type parameters end to end (ADR-0010): u0 overrides T with a keyword
// type, u1 with a parent typedef written as a bare identifier, u2 uses the
// default; the body localparam type and the U = T chained default reduce
// alongside. Every path must flatten to concrete widths.
module ttp_leaf #(parameter type T = logic [3:0], parameter type U = T)
  (input T din, output T dout);

  localparam type acc_t = logic [15:0];

  U mirror;
  acc_t acc;

  assign mirror = din;
  assign acc = {12'd0, mirror};
  assign dout = T'(acc);

endmodule

module tdef_type_param0(input logic [7:0] a, output logic [7:0] b,
                        output logic [7:0] c, output logic [3:0] d);

  typedef logic [7:0] word_t;

  ttp_leaf #(.T(logic [7:0])) u0 (.din(a), .dout(b));
  ttp_leaf #(.T(word_t)) u1 (.din(a), .dout(c));
  ttp_leaf u2 (.din(a[3:0]), .dout(d));

endmodule
