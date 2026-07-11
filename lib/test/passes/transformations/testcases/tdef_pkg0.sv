typedef logic [3:0] unit_t;

// Unit-scope typedef consumed through flattening: copied into each module,
// then substituted and dropped.
module tk_child(input unit_t i, output unit_t o);
  typedef unit_t chain_t;
  chain_t tmp;
  assign tmp = i;
  assign o = tmp;
endmodule

module tdef_pkg0(input logic [3:0] a, output logic [3:0] b);
  tk_child u(.i(a), .o(b));
endmodule
