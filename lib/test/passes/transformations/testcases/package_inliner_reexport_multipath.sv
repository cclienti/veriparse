package P1;

  localparam X = 10;

endpackage

package P2;

  import P1::*;
  localparam USED = X;
  export P1::*;

endpackage

module package_inliner_reexport_multipath
  (output [7:0] dout);

  import P1::*;
  import P2::*;

  localparam R = X;

  assign dout = R;

endmodule
