// A member path naming no member is a hard error.
module struct_err_member0(input logic [7:0] a, output logic [7:0] b);
  struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } v;
  always @(*) v = a;
  assign b = {v.hi, v.nope};
endmodule
