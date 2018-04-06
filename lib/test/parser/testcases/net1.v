module net1
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   tri                    a;
   tri signed [WIDTH-1:0] b;
   tri [WIDTH-1:0]        c [2**WIDTH-1:0];

endmodule
