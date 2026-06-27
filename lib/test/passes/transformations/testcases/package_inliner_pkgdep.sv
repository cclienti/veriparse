package P1;

  localparam A = 5;

endpackage

package P2;

  import P1::*;
  localparam B = A * 2;

endpackage

module package_inliner_pkgdep
  (output [7:0] dout);

  localparam C = P2::B;

  assign dout = C;

endmodule
