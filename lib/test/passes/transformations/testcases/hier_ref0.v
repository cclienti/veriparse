module hier_ref0;

  wire field;
  wire sel;
  wire a;
  wire b;

  // The leaf of a hierarchical reference matches the local declaration
  // 'field' by name but denotes another scope's signal: it must not be
  // renamed. The label index expression 'sel' is a plain local reference
  // and must be renamed.
  assign a = u1.field;
  assign b = u2[sel].field;

endmodule
