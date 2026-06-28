package P1;

  localparam A = 7;

endpackage

package P2;

  import P1::*;
  export P1::*;

endpackage

module package_inliner_reexport_shadow
  (output [7:0] dout);

  import P2::*;

  localparam A = 3;

  assign dout = A;

endmodule
