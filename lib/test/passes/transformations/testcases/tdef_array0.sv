module ta_leaf(input logic i, output logic o);
  assign o = ~i;
endmodule

// A typedef-typed actual on an instance array: after substitution the
// normalizer sees the concrete [3:0] type and splits element-wise —
// element k receives bit k, never a truncated scalar.
module tdef_array0(input logic [3:0] a, output logic [3:0] b);
  typedef logic [3:0] nib_t;
  nib_t av;
  assign av = a;
  ta_leaf u[3:0](.i(av), .o(b));
endmodule
