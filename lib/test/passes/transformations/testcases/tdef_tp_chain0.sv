// Chained parameterization (ADR-0010 §10): the outer module forwards its own
// type parameter with `.T(T)` — the reduced outer typedef makes the inner
// actual concrete before the inner binds.
module tcc_inner #(parameter type T = logic [1:0])(input T i, output T o);
  assign o = ~i;
endmodule

module tcc_outer #(parameter type T = logic [1:0])(input T i, output T o);
  tcc_inner #(.T(T)) u (.i(i), .o(o));
endmodule

module tdef_tp_chain0(input logic [7:0] a, output logic [7:0] b);
  tcc_outer #(.T(logic [7:0])) u (.i(a), .o(b));
endmodule
