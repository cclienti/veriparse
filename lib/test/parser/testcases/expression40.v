module expression40
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH:0]   in,
   output reg [WIDTH-1:0] out);

   always @(posedge clk) out <= in[WIDTH-1 -: WIDTH];

endmodule
