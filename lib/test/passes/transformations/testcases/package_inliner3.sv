package pa;

  localparam AVAL = 1;

endpackage

package pb;

  typedef logic [3:0] nib_t;

endpackage

module package_inliner3
  (output [3:0] dout);

  import pa::*;
  import pb::*;

  nib_t x;
  localparam K = AVAL;

  assign dout = x;

endmodule
