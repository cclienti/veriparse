package pc;

  localparam X = 99;

endpackage

module package_inliner_collision
  (output [7:0] dout);

  localparam X = 1;
  localparam Y = pc::X;

  assign dout = X + Y;

endmodule
