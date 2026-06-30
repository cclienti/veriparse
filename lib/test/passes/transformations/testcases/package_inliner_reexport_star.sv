package P1;

  localparam A = 1;
  localparam B = 2;

endpackage

package P2;

  import P1::*;
  localparam USE_A = A;
  localparam USE_B = B;
  export *::*;

endpackage

module package_inliner_reexport_star
  (output [7:0] dout);

  import P2::*;

  localparam R = A + B;

  assign dout = R;

endmodule
