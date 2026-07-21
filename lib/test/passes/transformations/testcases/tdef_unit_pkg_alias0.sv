// Bug repro (ADR-0009): a compilation-unit typedef whose aliased type is
// package-scoped. PackageInliner copies `bar_t` into the module but the
// clone's aliased NamedType still carries the `up_pkg` scope (the copy is
// spliced after refs are rewritten), so TypedefInliner sees an unresolved
// package-scoped type and aborts. The identical alias inside a module body
// resolves, so this must flatten too.
package up_pkg;
  typedef logic [7:0] foo_t;
endpackage

import up_pkg::*;
typedef up_pkg::foo_t bar_t;

module tdef_unit_pkg_alias0(input logic [7:0] a, output logic [7:0] b);
  bar_t x;
  assign x = a;
  assign b = x;
endmodule
