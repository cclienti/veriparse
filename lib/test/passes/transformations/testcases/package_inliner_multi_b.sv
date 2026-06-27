module package_inliner_multi_b
  (output [4:0] dout);

  localparam B = xpkg::XW;

  assign dout = B;

endmodule
