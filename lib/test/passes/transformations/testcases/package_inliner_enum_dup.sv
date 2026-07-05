package pedup_pkg;

  localparam PEDUP_A = 1'b0;
  typedef enum logic {PEDUP_A = 1'b1} t;

endpackage

module package_inliner_enum_dup (output logic o);

  import pedup_pkg::PEDUP_A;

  assign o = PEDUP_A;

endmodule
