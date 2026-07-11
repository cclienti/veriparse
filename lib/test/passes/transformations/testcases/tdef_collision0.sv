module tc_child(input logic [7:0] i, output logic [7:0] o);
  typedef logic [7:0] t;
  t tmp;
  assign tmp = i;
  assign o = tmp;
endmodule

// The child's `t` ([7:0]) and the top's `t` ([3:0]) must each substitute in
// their own scope; no typedef survives into the flat output.
module tdef_collision0(input logic [7:0] a, output logic [7:0] b, output logic [3:0] c);
  typedef logic [3:0] t;
  t narrow;
  assign narrow = a[3:0];
  assign c = narrow;
  tc_child u(.i(a), .o(b));
endmodule
