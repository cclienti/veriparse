// A packed array of aggregates (packed dims on the struct type itself) is
// rejected loudly rather than lowered at the wrong width.
module struct_err_packed_array0(input logic [15:0] a, output logic [15:0] b);
  struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } [1:0] pair;
  always @(*) pair = a;
  assign b = pair;
endmodule
