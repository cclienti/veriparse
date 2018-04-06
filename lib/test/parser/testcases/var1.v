module var1
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   integer a;
   integer b;
   integer c [2**WIDTH-1:0], h [63:0];
   integer d = 1, e, f = 2;

endmodule
