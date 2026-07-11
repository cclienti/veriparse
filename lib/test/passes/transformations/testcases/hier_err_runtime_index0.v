module hier_err_runtime_index0_leaf(input wire i, output wire o);

  wire sig;

  assign sig = i;
  assign o = sig;

endmodule


module hier_err_runtime_index0(input wire [3:0] in, input wire [1:0] sel,
                               output wire [3:0] out, output wire probe);

  hier_err_runtime_index0_leaf u[3:0] (.i(in), .o(out));

  // Runtime index into the split instance array: the index is not constant,
  // so it cannot select a unique flattened element and flattening must fail
  // loudly instead of leaving a dangling hierarchical reference.
  assign probe = u[sel].sig;

endmodule
