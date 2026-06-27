package P;

  localparam W = 8;
  typedef logic [W-1:0] data_t;

endpackage

module package_inliner_dep
  (output [7:0] dout);

  P::data_t d;

  assign dout = d;

endmodule
