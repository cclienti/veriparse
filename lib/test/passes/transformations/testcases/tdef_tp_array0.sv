// An array-typedef type actual: the alias's unpacked dims ride into the
// child's reduced typedef and land on the use-site declaration (ADR-0009 §5).
module tpa_leaf #(parameter type T = logic [7:0])
  (input logic [7:0] i, output logic [7:0] o);
  T store;
  always @(*) begin
    store[0] = i;
    store[1] = ~i;
  end
  assign o = store[0] ^ store[1];
endmodule

module tdef_tp_array0(input logic [7:0] a, output logic [7:0] b);
  typedef logic [7:0] mem_t [2];
  tpa_leaf #(.T(mem_t)) u (.i(a), .o(b));
endmodule
