package P1;

  localparam X = 42;

endpackage

package P2;

  import P1::X;
  export *::*;

endpackage

module package_inliner_reexport_star_explicit
  (output [7:0] dout);

  import P1::*;
  import P2::*;

  localparam R = X;

  assign dout = R;

endmodule
