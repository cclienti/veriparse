module expression47
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg [WIDTH-1:0] out);

   always @(posedge clk) out <= 3 / 2 + in[WIDTH/2-1 -: WIDTH/2] * in[0 +: WIDTH/2] / 2;

endmodule
