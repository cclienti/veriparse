// A scoped type actual (`.T(pkg::word_t)`, ADR-0010 §4): PackageInliner
// copies the package typedef into the parent and strips the scope, reducing
// to the bare-identifier case.
package tpq_pkg;
  typedef logic [7:0] word_t;
endpackage

module tpq_leaf #(parameter type T = logic [3:0])(input T i, output T o);
  assign o = ~i;
endmodule

module tdef_tp_pkg0(input logic [7:0] a, output logic [7:0] b);
  tpq_leaf #(.T(tpq_pkg::word_t)) u (.i(a), .o(b));
endmodule
