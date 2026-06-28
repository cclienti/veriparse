package pkg;

  localparam USED = 5;
  localparam UNUSED = 9;

endpackage

module package_inliner_lazy
  (output [7:0] dout);

  import pkg::*;

  localparam R = USED;

  assign dout = R;

endmodule
