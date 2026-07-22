// A unit typedef whose package alias depends on a package parameter: the
// dependency chain (word_t -> W) is pulled through the unit scope into the
// consuming module, and no dependency copy survives at top level.
package upp_pkg;
  localparam W = 8;
  typedef logic [W-1:0] word_t;
endpackage

typedef upp_pkg::word_t u_word_t;

module tdef_unit_pkg_param0(input logic [7:0] a, output logic [7:0] b);
  u_word_t x;
  assign x = a;
  assign b = x;
endmodule
