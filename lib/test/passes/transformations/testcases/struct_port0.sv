// A packed struct on child ports: the port lowers to a vector, the child's
// member accesses rewrite in its own resolution, and the flat design wires
// whole vectors.
module sp_child(input logic clk,
                input struct packed { logic [3:0] hi; logic [3:0] lo; } din,
                output logic [3:0] swapped_hi);
  logic [3:0] tmp;
  always @(posedge clk) tmp <= din.lo;
  assign swapped_hi = tmp;
endmodule

module struct_port0(input logic clk, input logic [7:0] a, output logic [3:0] y);
  sp_child u (.clk(clk), .din(a), .swapped_hi(y));
endmodule
