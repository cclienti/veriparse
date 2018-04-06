module net2
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   supply0                    a;
   supply0 signed [WIDTH-1:0] b;
   supply0 [WIDTH-1:0]        c [2**WIDTH-1:0];

endmodule
