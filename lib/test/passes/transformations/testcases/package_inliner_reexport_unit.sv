package PRU1;

  typedef logic [3:0] nib_t;

endpackage

package PRU2;

  import PRU1::*;
  export PRU1::nib_t;

endpackage

import PRU2::*;

// A `$unit::` qualified reference to a name the top-level wildcard import
// brings in through PRU2's re-export (§26.6): the `$unit` pseudo-package
// must snapshot PRU2's interface after its exports are folded, or the
// re-exported name is invisible to the qualified path.
module package_inliner_reexport_unit(input logic [3:0] a, output logic [3:0] b);
  $unit::nib_t x;
  assign x = a;
  assign b = x;
endmodule
