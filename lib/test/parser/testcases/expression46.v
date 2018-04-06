module expression46
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg [WIDTH-1:0] out);

   always @(posedge clk) out <= 3 * in*in + 2 - -1;

endmodule
