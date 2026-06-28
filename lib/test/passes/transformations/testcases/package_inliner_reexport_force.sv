package P1;

  localparam A = 7;
  localparam B = 9;

endpackage

package P2;

  import P1::*;
  export P1::A;

endpackage

module package_inliner_reexport_force
  (output [7:0] dout);

  import P2::*;

  localparam R = A;

  assign dout = R;

endmodule
