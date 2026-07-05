package ped_q_pkg;

  typedef enum logic [1:0] {PED_A = 2'h1, PED_B = 2'h2} state_t;

endpackage

package ped_p_pkg;

  import ped_q_pkg::state_t;
  export ped_q_pkg::*;

endpackage

module package_inliner_enum_diamond (output logic [1:0] o);

  import ped_p_pkg::*;
  import ped_q_pkg::*;

  assign o = PED_A;

endmodule
