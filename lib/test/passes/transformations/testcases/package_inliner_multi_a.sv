package xpkg;

  localparam XW = 5;

endpackage

import xpkg::*;

module package_inliner_multi_a
  (output [4:0] dout);

  localparam A = XW;

  assign dout = A;

endmodule
