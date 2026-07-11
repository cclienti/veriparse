module te_child(input logic i, output logic o);
  typedef enum logic [0:0] {TE_IDLE, TE_RUN} st_t;
  st_t st;
  always @(*) st = i ? TE_RUN : TE_IDLE;
  assign o = (st == TE_RUN);
endmodule

// A typedef'd enum in a child instantiated twice: the enum lowers to its
// base type, so the flat output declares no enumerator twice.
module tdef_enum0(input logic a, input logic b, output logic x, output logic y);
  te_child u0(.i(a), .o(x));
  te_child u1(.i(b), .o(y));
endmodule
