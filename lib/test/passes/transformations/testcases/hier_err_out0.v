module hier_err_out0_inner(input wire a, output wire z);
  wire y;
  assign y = a;
  assign z = y;
endmodule


module hier_err_out0_leaf(output wire q);
  assign q = 1'b1;
endmodule


module hier_err_out0(input wire a, output wire o);

  hier_err_out0_inner w(.a(a), .z(o));

  // The child output is connected to a hierarchical actual that is not an
  // interface member: flattening cannot drive it and must fail loudly rather
  // than emit a dangling reference.
  hier_err_out0_leaf u(.q(w.y));

endmodule
