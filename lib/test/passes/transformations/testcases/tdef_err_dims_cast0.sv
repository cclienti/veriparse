// An array typedef used where no unpacked-dims slot exists (a cast target)
// cannot substitute; the dims would be silently dropped.
module tdef_err_dims_cast0(input logic [7:0] a, output logic [7:0] b);
  typedef logic [7:0] arr_t [4];
  assign b = arr_t'(a);
endmodule
