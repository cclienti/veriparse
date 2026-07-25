// A select into a multi-packed-dim member is rejected loudly (the fold
// would mistake an element select for a bit select).
module struct_err_mdim_sel0(input logic [15:0] a, output logic [3:0] b);
  struct packed {
    logic [1:0][7:0] words;
  } s;
  always @(*) s = a;
  assign b = s.words[1];
endmodule
