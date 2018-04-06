module var2
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   real a;
   real b;
   real c [2**WIDTH-1:0], h [63:0] [47:0];
   real d = 1.0, e, f = 2.0;

endmodule
