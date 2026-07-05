package pie0_pkg;

  typedef enum logic [1:0] {PIE0_A = 2'h2, PIE0_B = 2'h3} pie0_t;

endpackage

module package_inliner_enum0 (output logic [1:0] o);

  import pie0_pkg::*;

  assign o = PIE0_A;

endmodule
