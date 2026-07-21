// Bug repro (ADR-0009): a compilation-unit typedef consumed inside an
// interface body. PackageInliner splices unit-scope typedefs into modules
// only (never interfaces) yet strips the top-level definition, so `byte_t`
// is gone by the time TypedefInliner walks the interface — it errors with
// "'byte_t' does not name a type". A module using the same alias flattens,
// so the interface must too.
typedef logic [7:0] byte_t;

interface ui_if;
  byte_t data;
  logic  valid;
endinterface

module tdef_unit_iface0(input logic [7:0] a, input logic v, output logic [7:0] b);
  ui_if bus();
  assign bus.data = a;
  assign bus.valid = v;
  assign b = bus.data;
endmodule
