package pie1_pkg;

  typedef enum logic [1:0] {PIE1_A = 2'h1, PIE1_B = 2'h2} pie1_t;

endpackage

module package_inliner_enum1 (output logic [1:0] a, output logic [1:0] b);

  pie1_pkg::pie1_t s;

  assign a = pie1_pkg::PIE1_A;
  assign b = pie1_pkg::PIE1_B;

endmodule
