module intconst1
  (input wire       reset,
   input wire       clk,
   input wire [7:0] in,
   output reg [7:0] out);

   always @(posedge clk) out <= 8'b0000_0000;

endmodule
