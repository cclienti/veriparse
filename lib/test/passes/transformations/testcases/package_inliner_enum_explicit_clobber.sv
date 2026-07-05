package peec_p_pkg;

  typedef enum logic {PEEC_A = 1'b0} e_t;

endpackage

package peec_q_pkg;

  typedef logic [7:0] e_t;

endpackage

module package_inliner_enum_explicit_clobber (output logic o);

  import peec_p_pkg::PEEC_A;
  import peec_q_pkg::e_t;

  e_t x;

  assign x = 8'd3;
  assign o = PEEC_A;

endmodule
