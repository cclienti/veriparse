package peds_pkg;

  typedef enum logic {PEDS_A = 1'b0} t;
  localparam W = PEDS_A;

endpackage

module package_inliner_enum_dep_shadow (output logic o);

  import peds_pkg::*;

  typedef enum logic {PEDS_A = 1'b1} local_t;

  local_t l;

  assign l = PEDS_A;
  assign o = W;

endmodule
