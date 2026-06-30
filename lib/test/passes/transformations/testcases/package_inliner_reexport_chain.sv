package P1;

  localparam X = 5;

endpackage

package P2;

  import P1::X;
  export P1::*;

endpackage

package P3;

  import P2::X;
  export P2::*;

endpackage

module package_inliner_reexport_chain
  (output [7:0] dout);

  import P3::*;

  localparam R = X;

  assign dout = R;

endmodule
