package pess_pkg;

  typedef enum logic {PESS_A = 1'b0} e_t;

endpackage

module package_inliner_enum_sibling_shadow (output logic o);

  import pess_pkg::*;

  typedef enum logic {PESS_X = 1'b0} e_t;

  e_t l;

  assign l = PESS_X;
  assign o = PESS_A;

endmodule
