package p1;

  localparam X = 1;
  localparam W = 4;

endpackage

package p2;

  localparam X = 2;

endpackage

module package_inliner_shadow
  (output [7:0] dout);

  import p1::*;
  import p2::X;

  localparam R = X;

  assign dout = R + W;

endmodule
