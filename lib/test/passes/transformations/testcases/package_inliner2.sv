package p2;

  localparam WIDTH = 8;
  localparam DEPTH = 16;

endpackage

module package_inliner2
  (output [7:0] dout);

  import p2::*;

  localparam WIDTH = 32;
  localparam D = DEPTH;

  assign dout = WIDTH + D;

endmodule
