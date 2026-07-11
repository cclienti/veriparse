// A body reference before the typedef's declaration point (IEEE 1800-2017
// 6.18): the later typedef is not yet bound when `t x;` substitutes.
module tdef_err_order0(input logic [3:0] a, output logic [3:0] b);
  t x;
  typedef logic [3:0] t;
  assign x = a;
  assign b = x;
endmodule
