package pa1;

  localparam X = 1;

endpackage

package pb1;

  localparam X = 2;

endpackage

module package_inliner_ambiguous
  (output [7:0] dout);

  import pa1::*;
  import pb1::*;

  localparam Z = X;

  assign dout = Z;

endmodule
