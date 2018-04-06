module net3
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   supply1             a;
   supply1 [WIDTH-1:0] b;
   supply1 [WIDTH-1:0] c [2**WIDTH-1:0];

endmodule
