module package_inliner_order_a
  (output [3:0] dout);

  localparam Z = latepkg::LW;

  assign dout = Z;

endmodule
