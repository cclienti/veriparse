package tp_pkg;
  typedef logic [3:0] nib_t;
endpackage

// A package typedef on child ports: the spliced typedef must be visible to
// the header, and the port substitutes to its concrete type.
module tp_child(input tp_pkg::nib_t i, output tp_pkg::nib_t o);
  assign o = i;
endmodule

module tdef_port0(input logic [3:0] a, output logic [3:0] b);
  tp_child u(.i(a), .o(b));
endmodule
