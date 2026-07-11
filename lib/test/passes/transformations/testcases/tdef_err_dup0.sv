module tdef_err_dup0(input logic [3:0] a, output logic [3:0] b);
  typedef logic [3:0] t;
  typedef logic [7:0] t;
  t x;
  assign x = a;
  assign b = x;
endmodule
