module ed_child(input logic i, output logic o);
  enum logic [0:0] {ED_IDLE, ED_RUN} st;
  always @(*) st = i ? ED_RUN : ED_IDLE;
  assign o = (st == ED_RUN);
endmodule

// A direct (non-typedef) enum decl in a child instantiated twice: same
// base-type lowering, no duplicate enumerator declarations.
module enum_dup0(input logic a, input logic b, output logic x, output logic y);
  ed_child u0(.i(a), .o(x));
  ed_child u1(.i(b), .o(y));
endmodule
