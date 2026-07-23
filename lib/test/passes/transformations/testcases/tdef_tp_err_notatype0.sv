// A value actual bound to a type formal (§23.10).
module ttn_leaf #(parameter type T = logic [3:0])(input T i, output T o);
  assign o = i;
endmodule

module tdef_tp_err_notatype0(input logic [3:0] a, output logic [3:0] b);
  ttn_leaf #(.T(4)) u (.i(a), .o(b));
endmodule
