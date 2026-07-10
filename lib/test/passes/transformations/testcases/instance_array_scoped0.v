module instance_array_scoped0_leaf(input wire i, output wire o);

  wire sig;

  assign sig = i;
  assign o = sig;

endmodule


module instance_array_scoped0(input wire [3:0] in, output wire [3:0] out,
                              output wire probe);

  instance_array_scoped0_leaf u[3:0] (.i(in), .o(out));

  // Indexed hierarchical reference into the split instance array: matches
  // the scalar element u2, flattened to u2_sig.
  assign probe = u[2].sig;

endmodule
