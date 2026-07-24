// A bodiless module with a header type parameter: the reduced typedef is
// spliced into a fresh body.
module tps_sink #(parameter type T = logic [7:0])(input T x);
endmodule

module tdef_tp_sink0(input logic [7:0] a, output logic [7:0] b);
  tps_sink #(.T(logic [7:0])) u (.x(a));
  assign b = a;
endmodule
