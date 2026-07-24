// A type actual whose packed width depends on a runtime signal (§6.20.2).
module tnc_leaf #(parameter type T = logic [3:0])(input T i, output T o);
  assign o = i;
endmodule

module tdef_tp_err_nonconst0(input logic [3:0] a, input logic [3:0] s,
                             output logic [3:0] b);
  tnc_leaf #(.T(logic [s-1:0])) u (.i(a), .o(b));
endmodule
