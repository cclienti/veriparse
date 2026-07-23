// A type actual bound to a value formal (§23.10).
module ttv_leaf #(parameter W = 4)(input logic [3:0] i, output logic [3:0] o);
  assign o = i;
endmodule

module tdef_tp_err_valuetype0(input logic [3:0] a, output logic [3:0] b);
  ttv_leaf #(.W(logic [7:0])) u (.i(a), .o(b));
endmodule
