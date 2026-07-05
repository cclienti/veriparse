package per_pkg;

  typedef enum logic [1:0] {PER_V[3]} v_t;
  typedef enum logic [1:0] {PER_W[6:4]} w_t;

endpackage

module package_inliner_enum_ranged (output logic [1:0] a, output logic [1:0] b);

  import per_pkg::*;

  assign a = PER_V1;
  assign b = per_pkg::PER_W5;

endmodule
