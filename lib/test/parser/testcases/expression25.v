module expression25
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg [WIDTH-1:0] out);

   always @(posedge clk) out <= in != 2;

endmodule
