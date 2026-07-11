typedef logic [3:0] nib_t;

import ipkg::*;

typedef nib_t pair_t;

module sv_unit_typedef0(input logic [3:0] a, output logic [3:0] b);
  nib_t x;
  assign x = a;
  assign b = x;
endmodule
