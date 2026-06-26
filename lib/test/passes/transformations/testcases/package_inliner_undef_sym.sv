package pe;

  localparam ONLY = 1;

endpackage

module package_inliner_undef_sym
  (output dout);

  import pe::missing;

  assign dout = 1'b0;

endmodule
