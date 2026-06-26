package p1;

  localparam A = 3;
  localparam B = 5;

endpackage

module package_inliner1
  (output [7:0] dout);

  import p1::A;

  localparam X = A;
  localparam Y = p1::B;

  assign dout = X + Y;

endmodule
