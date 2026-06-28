package P1;

  localparam A = 7;

endpackage

package Pbad;

  export P1::A;

endpackage

module package_inliner_reexport_err
  (output [7:0] dout);

  import Pbad::*;

  localparam B = A;

  assign dout = B;

endmodule
