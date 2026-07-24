// An unoverridden type parameter without a default (§6.20.3).
module tdef_tp_err_nodefault0 #(parameter type T)
  (input logic [3:0] a, output logic [3:0] b);
  T x;
  assign x = a;
  assign b = x;
endmodule
