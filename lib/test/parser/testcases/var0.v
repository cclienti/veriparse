module var0
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   reg                    a;
   reg signed [WIDTH-1:0] b;
   reg [WIDTH-1:0]        c [2**WIDTH-1:0], h [63:0] [31:0];
   reg [WIDTH-1:0]        d = 1, e, f = 2;

endmodule
